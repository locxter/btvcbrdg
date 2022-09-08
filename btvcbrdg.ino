#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WiFi, user authentication and serial related constants
const String WIFI_SSID = "";
const String WIFI_PASSWORD = "";
const String HOSTNAME = "btvcbrdg";
const char* USERNAME = "btvcbrdg";
const char* PASSWORD = "btvcbrdg";

// Server object
ESP8266WebServer server(80);

// Setup function
void setup(void) {
    // Initialize serial and WiFi connection
    Serial.begin(115200);
    Serial.setTimeout(10000);
    Serial.readString();
    Serial.setTimeout(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
            String content = "<!DOCTYPE html><html lang=\"en\"><head><title>btvcbrdg</title><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><meta name=\"author\" content=\"locxter\"><meta name=\"description\" content=\"This is an ESP8266 bridge for communicating to Neato Botvac robot vacuums over the network.\"><link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/water.css@2/out/water.min.css\"></head><body><header><h1>btvcbrdg</h1></header><main><form method=\"post\"><label for=\"command\">Command:</label><input id=\"command\" type=\"text\" name=\"command\"><input type=\"submit\" value=\"Send\"></form></main><footer><p>2022 locxter</p></footer></body></html>";
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