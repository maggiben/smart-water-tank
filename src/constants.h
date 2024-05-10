#pragma once
// WiFi Credentials
#define WIFI_SSID                   "TP-Link_42B4"
#define WIFI_PASSWORD               "pirulo123"

#define HOSTNAME                    "water-tank"
#define TIMEZONE                    "ART3ARST,M10.1.0/0,M3.3.0/0" // https://gist.github.com/tivaliy/a5ef9c7ccb4d57bdc248f0855c5eb1ff

// TRACE output simplified, can be deactivated here
#define TRACE(...)                  Serial.printf(__VA_ARGS__)
#define PRINT(...)                  Serial.print(__VA_ARGS__)
#define PRINTLN(...)                Serial.println(__VA_ARGS__)
#define JSONBOOL(value)             value ? F("true") : F("false")
