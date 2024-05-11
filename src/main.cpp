#include "main.h"

volatile bool shouldSleep = false;

void IRAM_ATTR onInterrupt() {
  TRACE("WakeUp!\n");
  shouldSleep = true;
}

void setup() {
  Serial.begin(115200);
  delay(150); // Add a short delay to stabilize serial communication

  // Wait for serial to initialize
  while (!Serial) {
    delay(150);
  }

  // Print "Hello, World!" to the serial port
  TRACE("Hello, World!\n");

  // Connect to WiFi
  if (connectToWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    ip = WiFi.localIP();
    TRACE("\n");
    TRACE("Connected: "); PRINT(ip); TRACE("\n");

    // Set up the OTA end callback
    ArduinoOTA.onEnd([]() {
      TRACE("OTA update successful, rebooting...\n");
      delay(500);
      ESP.restart();
    });

    ArduinoOTA.setHostname(HOSTNAME);
    // Initialize OTA
    ArduinoOTA.begin();

    // Enable CORS header in webserver results
    server.enableCORS(true);
    server.on("/api/heartbeat", HTTP_GET, []() {
      server.send(200, "text/plain", String(millis()));
    });
    server.begin();
    
  } else {
    TRACE("\n");  
    handleWifiConnectionError("WiFi connection error", true);
  }

  // pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(INTERRUPT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), onInterrupt, CHANGE);
}

void wakup() {
  TRACE("Wake Up!\n");
  // function for clearing all previously set wake interrupts:
  gpio_pin_wakeup_disable();
}

bool isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

bool connectToWiFi(const char* ssid, const char* password, int max_tries, int pause) {
  int i = 0;
  TRACE("Connectiong to Wifi\n");
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

void fpm_wakup_cb_func(void) {
  TRACE("Light sleep is over, either because timeout or external interrupt\n");
  Serial.flush();
}

void loop() {
  // Handle HTTP requests
  server.handleClient();
  // // Handle OTA updates
  ArduinoOTA.handle();
  TRACE("Hello, World Uptime: %lu !\n", (millis() / 1000));

  delay(1000);
}

void loopXXXXX() {
  static bool SHOULD_SLEEP = false;
  static unsigned long SLEEP_TIME_MINUTES = 1;
  // Handle HTTP requests
  server.handleClient();
  // // Handle OTA updates
  ArduinoOTA.handle();

  // Check if it's time to sleep
  if (!SHOULD_SLEEP && millis() >= SLEEP_TIME_MINUTES * 60 * 1000) {
    // TRACE("Going to sleep for %lu minutes\n", SLEEP_TIME_MINUTES);
    // delay(1000);
    SHOULD_SLEEP = true;
  }

  if (SHOULD_SLEEP) {
    SHOULD_SLEEP = false;
    Serial.println("Going to sleep");
    // Enter light sleep mode until interrupted
    ESP.deepSleepInstant(1000, WAKE_RF_DEFAULT);
    delay(10);
  }

  TRACE("Hello, World Uptime: %lu !\n", (millis() / 1000));
  delay(1000);
}

void loopXXX() {
  static bool SHOULD_SLEEP = false;
  static unsigned long SLEEP_TIME_MINUTES = 1;
  // Handle HTTP requests
  server.handleClient();
  // // Handle OTA updates
  ArduinoOTA.handle();
  TRACE("Hello, World Uptime: %lu !\n", (millis() / 1000));

  // Check if it's time to sleep
  if (!SHOULD_SLEEP && millis() >= SLEEP_TIME_MINUTES * 60 * 1000) {
    TRACE("Going to sleep for %lu minutes\n", SLEEP_TIME_MINUTES);
    delay(1000);
    SHOULD_SLEEP = true;
  }

  if (SHOULD_SLEEP) {
    SHOULD_SLEEP = false;
    // enable light sleep
    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
    wifi_fpm_open();

    // optionally, can register a callback function using
    wifi_fpm_set_wakeup_cb(wakup);

    // register one or more wake-up interrupts
    gpio_pin_wakeup_enable(INTERRUPT_PIN, GPIO_PIN_INTR_LOLEVEL);
    
    // actually enter light sleep:
    // the special timeout value of 0xFFFFFFF triggers indefinite
    // light sleep (until any of the GPIO interrupts above is triggered)
    wifi_fpm_do_sleep(0xFFFFFFF);
    delay(10);
  }

  wifi_fpm_close();
  // if (SHOULD_SLEEP) {
  //   SHOULD_SLEEP = false;
  //   // light sleep function requires microseconds
  //   wifi_fpm_do_sleep(SLEEP_TIME_MINUTES * 60 * 1000);

  //   // timed light sleep is only entered when the sleep command is
  //   // followed by a delay() that is at least 1ms longer than the sleep
  //   delay((SLEEP_TIME_MINUTES * 60 * 1000) + 1 );

  //   // ESP.deepSleep(SLEEP_TIME_MINUTES * 60 * 1000000); // SLEEP_TIME_MINUTES converted to microseconds
  //   // system_deep_sleep(SLEEP_TIME_MINUTES * 60 * 1000000);
  //   // system_deep_sleep(SLEEP_TIME_MINUTES * 60 * 1000000);
  //   // Serial.begin(115200);
    
  //   //   TRACE("SDK: %s\n", system_get_sdk_version());
  //   //   // Disable WiFi before sleep.
  //   //   wifi_station_disconnect();
  //   //   wifi_set_opmode(NULL_MODE);
  //   //   wifi_set_sleep_type(MODEM_SLEEP_T);

  //   TRACE("Sleeping for 10s\n");
  //   // system_deep_sleep(10 * 1000000);
  //   // system_deep_sleep(SLEEP_TIME_MINUTES * 60 * 1000000);

  //   //   // Wait for serial to initialize
    
  //   delay(100); // Add a short delay after waking up

  //   // Serial.begin(115200);
  //   // while (!Serial) {
  //   //   delay(150);
  //   // }
  // }
  delay(100);
}
