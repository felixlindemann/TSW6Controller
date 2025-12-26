#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"

// ---------------------------------------------------------------------------
// Erweiterte Captive-Portal-Implementierung für Pico-W
// Unterstützt: iOS/macOS, Android, Windows, ChromeOS
// ---------------------------------------------------------------------------

void setupCaptivePortal(WebServer &server, DNSServer &dnsServer, IPAddress apIP)
{
    String redirectUrl = "http://" + apIP.toString() + "/";
    
    // DNS leitet *alles* auf unseren AP um
    dnsServer.start(DEFAULT_DNS_PORT, "*", apIP);
    Serial.println("DNS-Server gestartet (Wildcard auf " + apIP.toString() + ")");

    // === Apple (iOS / macOS) ===
    // iOS prüft /hotspot-detect.html
    server.on("/hotspot-detect.html", [&server]()
              {
        Serial.println("[Captive] iOS/macOS Detect");
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.sendHeader("Pragma", "no-cache");
        server.sendHeader("Expires", "0");
        server.send(200, "text/html",
                    "<HTML><HEAD><TITLE>Success</TITLE></HEAD>"
                    "<BODY>Success</BODY></HTML>"); });

    // macOS CaptiveNetworkSupport
    server.on("/library/test/success.html", [&server]()
              {
        Serial.println("[Captive] macOS Library Test");
        server.sendHeader("Cache-Control", "no-cache");
        server.send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>"); });

    // === Android ===
    // Android prüft /generate_204

    server.on("/generate_204", [&server, redirectUrl]()
              {
        Serial.println("[Captive] Android Detect");
        server.sendHeader("Location", redirectUrl, true);
        server.send(302, "text/plain", ""); });

    // Alternative Android-Checks

    server.on("/gen_204", [&server, redirectUrl]()
              {
        Serial.println("[Captive] Android gen_204");
        server.sendHeader("Location", redirectUrl, true);
        server.send(302, "text/plain", ""); });

    // === Windows ===
    server.on("/ncsi.txt", [&server]()
              {
        Serial.println("[Captive] Windows NCSI");
        server.send(200, "text/plain", "Microsoft NCSI"); });


    server.on("/connecttest.txt", [&server, redirectUrl]()
              {
        Serial.println("[Captive] Windows ConnectTest");
        server.sendHeader("Location", redirectUrl, true);
        server.send(302, "text/plain", ""); });

    // === Chrome OS ===

    server.on("/connectivitycheck.html", [&server, redirectUrl]()
              {
        Serial.println("[Captive] ChromeOS Check");
        server.sendHeader("Location", redirectUrl, true);
        server.send(302, "text/plain", ""); });


    server.on("/generate204", [&server, redirectUrl]()
              {
        Serial.println("[Captive] ChromeOS generate204");
        server.sendHeader("Location", redirectUrl, true);
        server.send(302, "text/plain", ""); });

    // === Catch-All für nicht definierte Pfade ===
    server.onNotFound([&server, redirectUrl]()
                      {
        String uri = server.uri();
        Serial.printf("[Captive] Unbekannter Pfad: %s -> Redirect nach /\n", uri.c_str());
        // Manche Geräte brauchen http:// in der Location
        server.sendHeader("Location", redirectUrl, true);
        server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        server.send(302, "text/plain", ""); });

    Serial.println("Captive Portal vollständig aktiv");
}