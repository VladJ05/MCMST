#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Trebuie să setați SSID-ul și parola rețelei Wi-Fi
// De asemenea, trebuie să setați adresa IP a serverului

const char* ssid = "BSSID";
const char* password = "PSK";

const char* serverUrl = "https://ip.a.dd.r:5000/data";

WiFiClientSecure client;

const int sensorPin1 = 33; // E
const int sensorPin2 = 32; // S
const int sensorPin3 = 35; // W
const int sensorPin4 = 34; // N
const int ledPin = 22; // LED
int ledState = HIGH;

// Intervalul de trimitere a datelor (ms)
const unsigned long interval = 2000;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);

  pinMode(sensorPin1, INPUT_PULLUP);
  pinMode(sensorPin2, INPUT_PULLUP);
  pinMode(sensorPin3, INPUT_PULLUP);
  pinMode(sensorPin4, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
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
      http.begin(client, serverUrl);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(jsonString);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.print("Response: ");
        Serial.println(response);
        Serial.println("--------------------------------------------------------------------------------");

        StaticJsonDocument<256> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if(!error) {
          const char* status = responseDoc["status"];
          const char* message = responseDoc["message"];

          if (strcmp(status, "alert") == 0) {
            if(ledState == HIGH) {
              ledState = LOW;
              digitalWrite(ledPin, LOW);
            }
          } else if(strcmp(status, "success") == 0) {
            if(ledState == LOW) {
              ledState = HIGH;
              digitalWrite(ledPin, HIGH);
            }
          }
        } else {
          Serial.print("Error parsing response JSON: ");
          Serial.println(error.c_str());
        }
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

