
#pragma once
#include "config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include "ConfigStore.h"
#include "CaptivePortal.h"

// === Geräteinfos ===
const char *DEVICE_NAME = "TSW ContrOL²er";
const char *DEVICE_SSID = "TSWController-Setup";
const char *DEVICE_PASS = "tswsetup";
const char *MDNS_NAME = "TSWController";

WebServer server(80);
DNSServer dnsServer;
bool apMode = false;

// -------------------------------------------------------------
// mDNS
// -------------------------------------------------------------
void startMDNS()
{
    if (MDNS.begin(MDNS_NAME))
    {
        Serial.printf("mDNS aktiv: http://%s.local\n", MDNS_NAME);
    }
    else
    {
        Serial.println("Fehler: mDNS konnte nicht gestartet werden");
    }
}

// -------------------------------------------------------------
// Logo (Base64 aus LittleFS)
// -------------------------------------------------------------
bool checkLogoFile()
{
    if (!LittleFS.begin())
    {
        Serial.println("[LOGO] LittleFS mount fehlgeschlagen!");
        return false;
    }
    if (!LittleFS.exists("/logo_Ole_TSW.svg"))
    {
        Serial.println("[LOGO] Datei '/logo_Ole_TSW.svg' nicht gefunden!");
        return false;
    }
    File f = LittleFS.open("/logo_Ole_TSW.svg", "r");
    if (!f || f.size() == 0)
    {
        Serial.println("[LOGO] Datei existiert, ist aber leer oder unlesbar!");
        return false;
    }
    Serial.printf("[LOGO] Logo gefunden (%u Bytes)\n", f.size());
    f.close();
    return true;
}
 

// -------------------------------------------------------------
// HTML (Offline „Bootstrap“-Look) + JS für Mode-Umschaltung
// -------------------------------------------------------------
String buildPage(String title, String body)
{
    String html;
    html.reserve(9000);
    html += F("<!DOCTYPE html><html lang='de'><head><meta charset='UTF-8'>");
    html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    html += "<title>" + title + "</title>";
    html += F("<style>"
              "body{background:#f2f5fb;font-family:system-ui,Segoe UI,Roboto,sans-serif;"
              "margin:0;padding:0;color:#111;display:flex;justify-content:center;align-items:flex-start;min-height:100vh;}"
              ".wrap{width:90vw;max-width:520px;margin:24px auto;padding:0 8px;}"
              ".card{background:#fff;border-radius:14px;box-shadow:0 6px 22px rgba(0,0,0,.08);"
              "padding:32px 28px;margin:24px auto;box-sizing:border-box;animation:fadeInUp 0.8s ease-out 1;}"
              ".logo{text-align:center;margin:8px 0 16px 0}"
              ".logo img{max-width:220px;width:70%;height:auto;opacity:.98;"
              "animation:glowFade 2.8s ease-in-out 1;}" /* Logo-Glow */
              "@keyframes glowFade{"
              "  0%{filter:drop-shadow(0 0 0px rgba(13,110,253,0));opacity:.9;}"
              "  40%{filter:drop-shadow(0 0 10px rgba(13,110,253,.7));opacity:1;}"
              "  70%{filter:drop-shadow(0 0 18px rgba(13,110,253,.9));opacity:1;}"
              "  100%{filter:drop-shadow(0 0 0px rgba(13,110,253,0));opacity:.98;}"
              "}"
              "@keyframes fadeInUp{"
              "  0%{opacity:0;transform:translateY(20px);}"
              "  100%{opacity:1;transform:translateY(0);}"
              "}"
              "h1{font-size:1.5rem;margin:12px 0 20px 0;text-align:center;}"
              "label{display:block;margin-top:12px;font-weight:600}"
              "select,input{width:100%;font-size:1.05rem;padding:.75rem .8rem;"
              "border:1px solid #c9d4e5;border-radius:.6rem;background:#fff;box-sizing:border-box}"
              ".row{display:flex;gap:12px;flex-wrap:wrap}"
              ".row>div{flex:1}"
              ".btnbar{display:flex;gap:12px;justify-content:center;margin-top:20px;flex-wrap:wrap}"
              ".btn{display:inline-block;text-align:center;padding:.85rem 1.3rem;border-radius:.7rem;"
              "border:1px solid transparent;cursor:pointer;font-size:1.05rem;min-width:140px;}"
              ".btn-primary{background:#0d6efd;color:#fff}"
              ".btn-ghost{background:#fff;border-color:#cfd8ea;color:#222}"
              ".nav{display:flex;gap:20px;justify-content:center;margin:10px 0 0 0;flex-wrap:wrap}"
              ".nav a{color:#0d6efd;text-decoration:none;font-weight:600}"
              ".nav a:hover{text-decoration:underline}"
              ".hint{font-size:.9rem;opacity:.75;margin-top:14px;text-align:center;word-break:break-word}"
              ".spacer{height:4px}"
              "@media (max-width:420px){"
              "  h1{font-size:1.25rem;}"
              "  .card{padding:24px 18px;}"
              "  .logo img{max-width:180px;}"
              "}"
              "</style>");

    html += F(""
              "<script>"
              "function onModeChange(){"
              "var mode=document.getElementById('mode').value;"
              "var wifi=document.getElementById('wifiBlock');"
              "wifi.style.display=(mode==='sta')?'block':'none';"
              "}"
              "document.addEventListener('DOMContentLoaded',()=>{"
              "  const img=document.querySelector('.logo img');"
              "  if(img){img.style.animation='none';void img.offsetWidth;"
              "  img.style.animation='glowFade 2.8s ease-in-out 1';}"
              "});"
              "document.addEventListener('DOMContentLoaded',onModeChange);"
              "</script>"
              "</head><body><div class='wrap'>");
    html += "<div class='nav'><a href='/'>Setup</a>  <a href='/status'>Status</a></div>";
    html += "<div class='card'>" + body + "</div>";
    html += "<div class='hint'>Erreiche mich auch unter "
            "<a href='http://" +
            String(MDNS_NAME) + ".local'>" + String(MDNS_NAME) + ".local</a></div>";
    html += "</div></body></html>";
    return html;
}

// -------------------------------------------------------------
// Seiten
// -------------------------------------------------------------
void buildSetupForm(String &out)
{
    // Logo + Titel
    // String logo = embedLogo();
    out += "<div class='logo'>";
    /*if (logo.length())
      out += "<img src='data:image/svg+xml;base64," + logo + "' alt='TSW Logo'>";
      */

    out += "<img src='/logo_Ole_TSW.svg' alt='TSW Logo'>";

    out += "</div>";
    out += "<h1>" + String(DEVICE_NAME) + " Setup</h1>";

    // Formular
    // WLAN-Scan
    int n = WiFi.scanNetworks();

    out += "<form method='POST' action='/save'>";

    // 1) Modus
    out += "<label>Betriebsart</label>";
    out += "<select id='mode' name='mode' onchange='onModeChange()'>";
    if (cfg.apModePreferred)
    {
        out += "<option value='sta'>Client</option><option value='ap' selected>Access Point</option>";
    }
    else
    {
        out += "<option value='sta' selected>Client</option><option value='ap'>Access Point</option>";
    }
    out += "</select>";

    // 2) WLAN + Passwort (nur im Client-Modus sichtbar)
    out += "<div id='wifiBlock' style='display:none'>";
    out += "<label>WLAN auswählen</label>";
    out += "<select class='form-select' name='ssid'>";
    for (int i = 0; i < n; i++)
    {
        String ss = WiFi.SSID(i);
        ss.replace("&", "&amp;");
        ss.replace("<", "&lt;");
        ss.replace(">", "&gt;");
        out += "<option value='" + ss + "'";
        if (strlen(cfg.ssid) && ss == String(cfg.ssid))
            out += " selected";
        out += ">" + ss + "</option>";
    }
    out += "</select>";

    out += "<label>Passwort</label>";
    out += "<input type='password' class='form-control' name='pass' value='" + String(cfg.pass) + "'>";
    out += "</div>"; // wifiBlock

    // 3) API-Key
    out += "<label>TSW API Key</label>";
    out += "<input type='text' class='form-control' name='api' value='" + String(cfg.apiKey) + "'>";

    // Buttons
    out += "<div class='btnbar'>";
    out += "<button type='submit' class='btn btn-primary'>Speichern & Neustarten</button>";
    out += "<a class='btn btn-ghost' href='/status'>Status</a>";
    out += "</div>";

    out += "</form>";
}

void handleRoot()
{
    String body;
    body.reserve(7000);
    buildSetupForm(body);
    server.send(200, "text/html", buildPage("Setup - " + String(DEVICE_NAME), body));
}

void handleConfigPage()
{
    String body;
    body.reserve(7000);
    buildSetupForm(body); // identisch zur Setup-Seite
    server.send(200, "text/html", buildPage("Config - " + String(DEVICE_NAME), body));
}

void handleStatusPage()
{
    String body;
    body += "<div class='logo'>";
    /*
    String logo = embedLogo();

    if (logo.length())
      body += "<img src='data:image/svg+xml;base64," + logo + "' alt='TSW Logo'>";
       */
    body += "<img src='/logo_Ole_TSW.svg' alt='TSW Logo'>";

    body += "</div>";
    body += "<h1>Status</h1>";
    body += "<div class='spacer'></div>";
    body += "<label>Modus</label><div>" + String(apMode ? "Access Point" : "Client") + "</div>";
    String currSsid = WiFi.SSID();
    if (!currSsid.length())
        currSsid = cfg.ssid;
    body += "<label>SSID</label><div>" + currSsid + "</div>";
    body += "<label>IP</label><div>" + String(apMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString()) + "</div>";
    body += "<label>Signal (RSSI)</label><div>" + String(WiFi.RSSI()) + " dBm</div>";
    body += "<div class='btnbar'><a class='btn btn-ghost' href='/reboot'>Neustart</a></div>";
    server.send(200, "text/html", buildPage("Status - " + String(DEVICE_NAME), body));
}

// -------------------------------------------------------------
// Speichern & Neustart
// -------------------------------------------------------------
void handleSave()
{
    String mode = server.arg("mode");
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    String api = server.arg("api");

    cfg.apModePreferred = (mode == "ap");
    ssid.toCharArray(cfg.ssid, sizeof(cfg.ssid));
    pass.toCharArray(cfg.pass, sizeof(cfg.pass));
    api.toCharArray(cfg.apiKey, sizeof(cfg.apiKey));
    saveConfig();

    server.send(200, "text/html",
                "<div style='font-family:system-ui,Segoe UI,Roboto,sans-serif;padding:1em;'>"
                "<h3>Gespeichert.</h3><p>Gerät startet neu...</p></div>");
    delay(1200);
    ESP.restart();
}

void handleReboot()
{
    server.send(200, "text/html",
                "<div style='font-family:system-ui,Segoe UI,Roboto,sans-serif;padding:1em;'>"
                "<h3>Neustart...</h3></div>");
    delay(400);
    ESP.restart();
}

void serverGetLogo()
{
    // Logo-Datei direkt aus LittleFS streamen
    server.on("/logo_Ole_TSW.svg", []()
              {
  if (!LittleFS.begin()) {
    server.send(500, "text/plain", "LittleFS mount failed");
    return;
  }
  File f = LittleFS.open("/logo_Ole_TSW.svg", "r");
  if (!f) {
    server.send(404, "text/plain", "Logo not found");
    return;
  }
  server.streamFile(f, "image/svg+xml");
  f.close(); });
}

// -------------------------------------------------------------
// AP-Mode
// -------------------------------------------------------------
void startAPMode()
{
    apMode = true;
    digitalWrite(STATUS_LED, LOW);
    WiFi.mode(WIFI_AP);

    Serial.println("\n=== STARTE ACCESS POINT ===");
    WiFi.softAP(DEVICE_SSID, DEVICE_PASS);
    delay(300);
    // IPAddress apIP(192, 168, 4, 1);
    IPAddress apIP = IPAddress();
    apIP.fromString(MyIP);

    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    delay(200);

    // Captive Portal + Routen
    setupCaptivePortal(server, dnsServer, apIP);
    server.on("/", handleRoot);
    server.on("/config", handleConfigPage);
    server.on("/status", handleStatusPage);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/reboot", handleReboot);
    serverGetLogo();
    server.begin();
    startMDNS();

    Serial.println("Webserver läuft auf http://" MyIP " sowie http://TSWController.local");
}

// -------------------------------------------------------------
// Client-Mode (STA)
// -------------------------------------------------------------
void startNormalMode()
{
    apMode = false;
    WiFi.mode(WIFI_STA);
    WiFi.begin(cfg.ssid, cfg.pass);
    digitalWrite(STATUS_LED, HIGH);

    Serial.printf("Verbinde mit %s ...\n", cfg.ssid);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 12000)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nVerbunden!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        server.on("/", handleRoot);
        server.on("/config", handleConfigPage);
        server.on("/status", handleStatusPage);
        server.on("/save", HTTP_POST, handleSave);
        server.on("/reboot", handleReboot);
        serverGetLogo();
        server.begin();
        startMDNS();
    }
    else
    {
        Serial.println("\nFehler -> Fallback in AP-Modus");
        startAPMode();
    }
}

// -------------------------------------------------------------
// Startlogik & Loop
// -------------------------------------------------------------
void beginWiFiManager()
{
    pinMode(SETUP_BUTTON, INPUT_PULLUP);
    pinMode(STATUS_LED, OUTPUT);
    checkLogoFile();

    bool setupPressed = (digitalRead(SETUP_BUTTON) == LOW);
    if (setupPressed)
    {
        startAPMode();
    }
    else if (loadConfig())
    {
        // Mit vorhandener Config den bevorzugten Modus fahren
        if (cfg.apModePreferred)
            startAPMode();
        else
            startNormalMode();
    }
    else
    {
        // Keine Config vorhanden -> immer AP
        startAPMode();
    }
}

void loopWiFiManager()
{
    dnsServer.processNextRequest();
    server.handleClient();

    static unsigned long last = 0;
    if (millis() - last > 10000)
    {
        last = millis();
        Serial.printf("=== HEARTBEAT ===  Modus: %s | IP: %s\n",
                      apMode ? "AP" : "Client",
                      (apMode ? WiFi.softAPIP() : WiFi.localIP()).toString().c_str());
    }
}
