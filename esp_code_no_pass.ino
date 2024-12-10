#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "-----";
const char* password = "-----";

const char* serverUrl = "http://192.168.1.128:5000/data";

const int sensorPin1 = 33; // E
const int sensorPin2 = 32; // S
const int sensorPin3 = 35; // W
const int sensorPin4 = 34; // N

// Intervalul de trimitere a datelor (ms)
const unsigned long interval = 2000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);

  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(sensorPin3, INPUT_PULLUP);
  pinMode(sensorPin4, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float sensorValue1 = analogRead(sensorPin1);
    float sensorValue2 = analogRead(sensorPin2);
    float sensorValue3 = analogRead(sensorPin3);
    float sensorValue4 = analogRead(sensorPin4);

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["sensor_E"] = sensorValue1;
    jsonDoc["sensor_S"] = sensorValue2;
    jsonDoc["sensor_W"] = sensorValue3;
    jsonDoc["sensor_N"] = sensorValue4;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(jsonString);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error sending POST: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
      }

      http.end();
    } else {
      Serial.println("Wi-Fi not connected. Skipping data send.");
    }

    // Debugging: afișează datele citite în serial monitor
    Serial.println(jsonString);
  }
}

