#include "main.h"

void setup() {
  Serial.begin(115200);

  // Wait for serial to initialize
  while (!Serial) {
    delay(150);
  }

  // Print "Hello, World!" to the serial port
  TRACE("Hello, World!\n");
  if (connectToWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    ip = WiFi.localIP();
    TRACE("\n");
    TRACE("Connected: "); PRINT(ip); TRACE("\n");

    // // Set up the OTA end callback
    ArduinoOTA.onEnd([]() {
      TRACE("OTA update successful, rebooting...\n");
      delay(500);
      ESP.restart();
    });

    ArduinoOTA.setHostname(HOSTNAME);
    // Initialize OTA
    ArduinoOTA.begin();  

  } else {
    TRACE("\n");  
    handleWifiConnectionError("WiFi connection error", true);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  TRACE("Hello, World loop!\n");
  delay(1000);
}



bool isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

bool connectToWiFi(const char* ssid, const char* password, int max_tries, int pause) {
  int i = 0;
  // allow to address the device by the given name e.g. http://webserver
  WiFi.setHostname(HOSTNAME);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  
  #if defined(ARDUINO_ARCH_ESP8266)
    WiFi.forceSleepWake();
    delay(200);
  #endif
  WiFi.begin(ssid, password);
  do {
    delay(pause);
    TRACE(".");
    i++;
  } while (!isConnected() && i < max_tries);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // Initialize mDNS
  while (!MDNS.begin(HOSTNAME)) {
    TRACE("Error setting up MDNS responder!\"");
    delay(1000);
  }

  return isConnected();
}


void handleWifiConnectionError(String error, bool restart) {
  TRACE("Error: %s\n", error.c_str());
  TRACE("Rebooting now...\n");
  delay(1500);
  ESP.restart();
}
