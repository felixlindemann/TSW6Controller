#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>

class NetTracer {
public:
    void begin(uint16_t udpPort = 68) {
        Serial.println("[TRACE] NetTracer initialisiert...");
        udp.begin(udpPort);
        Serial.printf("[TRACE] Lausche UDP-Port %u\n", udpPort);
    }

    // Raw UDP listener
    void loop() {
        int packetSize = udp.parsePacket();
        if (packetSize > 0) {
            IPAddress remote = udp.remoteIP();
            uint16_t rport = udp.remotePort();
            Serial.printf("[TRACE][UDP] %d bytes von %s:%u\n",
                          packetSize, remote.toString().c_str(), rport);

            static uint8_t buf[512];
            int len = udp.read(buf, sizeof(buf));
            if (len > 0) {
                Serial.print("         HEX: ");
                for (int i = 0; i < len; i++) {
                    Serial.printf("%02X ", buf[i]);
                    if (i >= 63) { Serial.print("..."); break; }
                }
                Serial.println();
            }
        }
    }

    // Hook für WebServer (HTTP)
    void attachToServer(WebServer& server) {
        server.onNotFound([&server, this]() {
            Serial.printf("[TRACE][HTTP] %s -> %s\n",
                          server.client().remoteIP().toString().c_str(),
                          server.uri().c_str());
            server.sendHeader("Location", "/", true);
            server.send(302, "text/plain", "");
        });
    }

private:
    WiFiUDP udp;
};
