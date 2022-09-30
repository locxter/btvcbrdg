#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WiFi and user authentication related constants
const String SSID = "";
const String WIFI_PASSWORD = "";
const String HOSTNAME = "btvcbrdg";
const char* USERNAME = "btvcbrdg";
const char* PASSWORD = "btvcbrdg";

// Server object
ESP8266WebServer server(80);

// Setup function
void setup(void) {
    // Initialize filesystem 
    LittleFS.begin();
    // Initialize serial and WiFi connection
    Serial.begin(115200);
    Serial.setTimeout(10000);
    Serial.readString();
    Serial.setTimeout(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    // Initialize mDNS
    MDNS.begin(HOSTNAME);
    // Initialize server
    server.on("/", HTTP_GET, [&]() {
        if (!server.authenticate(USERNAME, PASSWORD)) {
            server.requestAuthentication();
        } else {
            File file = LittleFS.open("/index.html", "r");
            String content;
            while (file.available()) {
                content += (char) file.read();
            }
            file.close();
            server.send(200, "text/html", content);
        }
    });
    server.on("/", HTTP_POST, [&]() {
        if (!server.authenticate(USERNAME, PASSWORD)) {
            server.requestAuthentication();
        } else {
            String content = "";
            if (server.hasArg("command")) {
                Serial.print(server.arg("command"));
                Serial.print('\n');
                content = Serial.readStringUntil('\x1a');
            }
            server.send(200, "text/plain", content);
        }
    });
    server.serveStatic("/styles/water.min.css", LittleFS, "/styles/water.min.css");
    server.onNotFound([&]() {
        server.send(404, "text/plain", "Resource not found");
    });
    server.begin();
}

// Main function
void loop(void) {
    server.handleClient();
    MDNS.update();
}