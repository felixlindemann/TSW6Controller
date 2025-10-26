#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP dhcpUDP;

struct Lease {
  uint8_t mac[6];
  IPAddress ip;
  unsigned long lastSeen;
  bool active;
};

#define DHCP_PORT_SERVER 67
#define DHCP_PORT_CLIENT 68
#define MAX_LEASES 8
#define LEASE_TIMEOUT 60000UL

static Lease leases[MAX_LEASES];
static IPAddress routerIP(192,168,4,1);
static uint8_t nextHost = 2;

IPAddress nextIP() {
  if (nextHost > 10) nextHost = 2;
  return IPAddress(routerIP[0], routerIP[1], routerIP[2], nextHost++);
}

void printMAC(const uint8_t* m) {
  char buf[18];
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
          m[0],m[1],m[2],m[3],m[4],m[5]);
  Serial.print(buf);
}

void startMiniDHCP(IPAddress apIP) {
  routerIP = apIP;
  dhcpUDP.begin(DHCP_PORT_SERVER);   // KORREKTUR: Server lauscht auf Port 67!
  memset(leases, 0, sizeof(leases));
  Serial.println("Mini-DHCP-Server gestartet (Port 67)");
}

Lease* findOrCreateLease(uint8_t* mac) {
  for (int i=0;i<MAX_LEASES;i++)
    if (leases[i].active && memcmp(leases[i].mac,mac,6)==0) return &leases[i];
  for (int i=0;i<MAX_LEASES;i++)
    if (!leases[i].active) {
      memcpy(leases[i].mac,mac,6);
      leases[i].ip = nextIP();
      leases[i].active = true;
      leases[i].lastSeen = millis();
      Serial.print("Lease vergeben: ");
      printMAC(mac);
      Serial.print(" -> ");
      Serial.println(leases[i].ip);
      return &leases[i];
    }
  return nullptr;
}

void handleMiniDHCP() {
  int size = dhcpUDP.parsePacket();
  if (size <= 0) return;

  Serial.printf("[DHCP] Paket empfangen: %d bytes von %s:%d\n", 
                size, dhcpUDP.remoteIP().toString().c_str(), dhcpUDP.remotePort());

  uint8_t buf[548];
  int bytesRead = dhcpUDP.read(buf, sizeof(buf));
  
  if (bytesRead < 240) {
    Serial.println("[DHCP] Paket zu klein, ignoriert");
    return;
  }

  // DHCP Message Type prüfen
  uint8_t msgType = 0;
  for (int i = 236; i < bytesRead - 3; i++) {
    if (buf[i] == 53 && buf[i+1] == 1) {  // Option 53 = Message Type
      msgType = buf[i+2];
      break;
    }
  }
  
  Serial.printf("[DHCP] Message Type: %d (1=DISCOVER, 3=REQUEST)\n", msgType);

  // MAC-Adresse extrahieren (chaddr field)
  uint8_t mac[6];
  memcpy(mac, buf + 28, 6);
  
  Serial.print("[DHCP] Client MAC: ");
  printMAC(mac);
  Serial.println();

  Lease* lease = findOrCreateLease(mac);
  if (!lease) {
    Serial.println("[DHCP] Keine freien Leases mehr!");
    return;
  }

  // DHCP-Antwort aufbauen
  uint8_t reply[300];
  memset(reply, 0, sizeof(reply));
  
  // Header kopieren (op, htype, hlen, hops)
  reply[0] = 2;  // BOOTREPLY
  memcpy(reply + 1, buf + 1, 3);
  
  // Transaction ID kopieren
  memcpy(reply + 4, buf + 4, 4);
  
  // Zeiten
  memcpy(reply + 8, buf + 8, 2);  // secs
  memcpy(reply + 10, buf + 10, 2); // flags
  
  // IP-Adressen
  *(uint32_t*)(reply + 16) = (uint32_t)lease->ip;      // yiaddr (your IP)
  *(uint32_t*)(reply + 20) = (uint32_t)routerIP;       // siaddr (server IP)
  
  // Client MAC
  memcpy(reply + 28, mac, 6);
  
  // Magic Cookie
  reply[236] = 0x63;
  reply[237] = 0x82;
  reply[238] = 0x53;
  reply[239] = 0x63;
  
  int idx = 240;
  
  // Option 53: Message Type (DHCPOFFER=2 oder DHCPACK=5)
  reply[idx++] = 53;
  reply[idx++] = 1;
  reply[idx++] = (msgType == 1) ? 2 : 5;  // OFFER oder ACK
  
  // Option 1: Subnet Mask
  reply[idx++] = 1;
  reply[idx++] = 4;
  reply[idx++] = 255;
  reply[idx++] = 255;
  reply[idx++] = 255;
  reply[idx++] = 0;
  
  // Option 3: Router
  reply[idx++] = 3;
  reply[idx++] = 4;
  *(uint32_t*)(reply + idx) = (uint32_t)routerIP;
  idx += 4;
  
  // Option 6: DNS Server
  reply[idx++] = 6;
  reply[idx++] = 4;
  *(uint32_t*)(reply + idx) = (uint32_t)routerIP;
  idx += 4;
  
  // Option 51: Lease Time (60 Sekunden)
  reply[idx++] = 51;
  reply[idx++] = 4;
  reply[idx++] = 0;
  reply[idx++] = 0;
  reply[idx++] = 0;
  reply[idx++] = 60;
  
  // Option 54: DHCP Server Identifier
  reply[idx++] = 54;
  reply[idx++] = 4;
  *(uint32_t*)(reply + idx) = (uint32_t)routerIP;
  idx += 4;
  
  // Option 255: End
  reply[idx++] = 255;

  // Antwort senden (KORREKTUR: an Port 68!)
  dhcpUDP.beginPacket(IPAddress(255,255,255,255), DHCP_PORT_CLIENT);
  dhcpUDP.write(reply, idx);
  dhcpUDP.endPacket();

  Serial.printf("[DHCP] %s gesendet an ", (msgType == 1) ? "OFFER" : "ACK");
  Serial.println(lease->ip);

  lease->lastSeen = millis();

  // Alte Leases aufräumen
  for (int i=0;i<MAX_LEASES;i++)
    if (leases[i].active && millis()-leases[i].lastSeen>LEASE_TIMEOUT)
      leases[i].active = false;
}