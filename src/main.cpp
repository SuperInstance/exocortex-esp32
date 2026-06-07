// Exocortex ESP32 Sensor Node
// Reads analog sensors and communicates with the Exocortex API
// POSTs to /tap/sense, GETs /tap/recall and /tap/predict

#include <WiFi.h>
#include <HTTPClient.h>

// --- Configuration ---
// Override with build flags: -DWIFI_SSID=\"xxx\" -DWIFI_PASS=\"xxx\" -DCORTEX_URL=\"http://host:port\"
#ifndef WIFI_SSID
#define WIFI_SSID "YOUR_SSID"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "YOUR_PASSWORD"
#endif
#ifndef CORTEX_URL
#define CORTEX_URL "http://192.168.1.100:9000"
#endif

#define TEMP_PIN A0       // Temperature sensor (analog)
#define MOIST_PIN A1      // Moisture sensor (analog)
#define SENSE_INTERVAL 30000  // 30 seconds
#define MAX_RESPONSE 512

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* cortexUrl = CORTEX_URL;

WiFiClient client;
HTTPClient http;
unsigned long lastSense = 0;

// --- Sensor Reads ---
// TMP36-style: 0-3.3V → 0-4095 ADC
float readTemperature() {
  int raw = analogRead(TEMP_PIN);
  float voltage = raw * (3.3 / 4096.0);
  // TMP36: 500mV = 0°C, 10mV/°C
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;
}

// Capacitive soil moisture: 0=dry, 4095=wet → map to 0-100%
float readMoisture() {
  int raw = analogRead(MOIST_PIN);
  float pct = map(raw, 4095, 0, 0, 100);
  return pct;
}

// --- WiFi ---
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi FAILED");
  }
}

// --- POST /tap/sense ---
void postSense(float temp, float humidity, int zone) {
  String url = String(cortexUrl) + "/tap/sense";
  http.begin(client, url);
  http.setTimeout(5000);

  char body[64];
  snprintf(body, sizeof(body), "t:%.1f h:%.0f z:%d", temp, humidity, zone);

  http.addHeader("Content-Type", "text/plain");
  int code = http.POST(body);
  if (code > 0) {
    String resp = http.getString();
    Serial.printf("  sense → %d: %s\n", code, resp.c_str());
  } else {
    Serial.printf("  sense FAILED: %s\n", http.errorToString(code).c_str());
  }
  http.end();
}

// --- GET /tap/recall?q=... ---
String recallMemory(const String& query) {
  String url = String(cortexUrl) + "/tap/recall?q=" + urlEncode(query);
  http.begin(client, url);
  http.setTimeout(5000);

  int code = http.GET();
  String result = "";
  if (code > 0) {
    result = http.getString();
    Serial.printf("  recall → %d: %s\n", code, result.c_str());
  } else {
    Serial.printf("  recall FAILED: %s\n", http.errorToString(code).c_str());
  }
  http.end();
  return result;
}

// --- GET /tap/predict?sensor=...&reading=... ---
String predict(const String& sensor, float reading) {
  char urlBuf[256];
  snprintf(urlBuf, sizeof(urlBuf), "%s/tap/predict?sensor=%s&reading=%.2f",
           cortexUrl, sensor.c_str(), reading);
  http.begin(client, urlBuf);
  http.setTimeout(5000);

  int code = http.GET();
  String result = "";
  if (code > 0) {
    result = http.getString();
    Serial.printf("  predict → %d: %s\n", code, result.c_str());
  } else {
    Serial.printf("  predict FAILED: %s\n", http.errorToString(code).c_str());
  }
  http.end();
  return result;
}

// Simple URL encoding for query strings
String urlEncode(const String& str) {
  String encoded = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else if (c == ' ') {
      encoded += '+';
    } else {
      char hex[4];
      snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
      encoded += hex;
    }
  }
  return encoded;
}

// --- Demo: run recall + predict on boot ---
void demoEndpoints() {
  Serial.println("\n--- Demo: recall ---");
  recallMemory("temperature yesterday");

  Serial.println("--- Demo: predict ---");
  predict("temperature", 22.5);
  Serial.println("--- Demo done ---\n");
}

// --- Arduino Lifecycle ---
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Exocortex ESP32 Sensor Node ===");
  Serial.printf("Cortex: %s\n", cortexUrl);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    demoEndpoints();
  }
}

void loop() {
  // Reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
    delay(5000);
    return;
  }

  unsigned long now = millis();
  if (now - lastSense >= SENSE_INTERVAL || lastSense == 0) {
    lastSense = now;

    float temp = readTemperature();
    float moist = readMoisture();
    int zone = 3; // Default zone, could be configurable

    Serial.printf("Read: temp=%.1f°C  moisture=%.0f%%  zone=%d\n", temp, moist, zone);
    postSense(temp, moist, zone);
  }

  delay(100); // Light sleep
}
