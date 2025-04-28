#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "Siva M31s"; //Enter your Wifi router Name
const char* password = "12345678"; // Wifi password

WebSocketsClient webSocket;

const int ir1Pin = 5;  // D1
const int ir2Pin = 4;  // D2
const int vibrationPin = A0;

unsigned long startTime = 0;
bool objectDetected = false;

void setup() {
  Serial.begin(115200);
  pinMode(ir1Pin, INPUT);
  pinMode(ir2Pin, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    attempt++;
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.print(" - Status Code: ");
    Serial.println(WiFi.status());  
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // WebSocket setup
  webSocket.begin("192.168.13.7", 8765, "/");  // Replace with your server IP
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();

  int ir1State = digitalRead(ir1Pin);
  int ir2State = digitalRead(ir2Pin);
  int vibration = analogRead(vibrationPin);


  if (ir1State == LOW && !objectDetected) {
    objectDetected = true;
    startTime = millis();
    Serial.println("IR1 triggered - Timer started");
  }


  if (ir2State == LOW && objectDetected) {
    objectDetected = false;
    unsigned long travelTime = millis() - startTime;
    float travelTimeSec = travelTime / 1000.0;

    StaticJsonDocument<200> doc;
    doc["action"] = "set_travel_time";
    doc["value"] = travelTimeSec;
    doc["vibration_warning"] = (vibration > 700);

    String jsonData;
    serializeJson(doc, jsonData);
    webSocket.sendTXT(jsonData);
    Serial.println("Sent travel time: " + jsonData);
  }


  if (vibration > 700) {
    StaticJsonDocument<100> alert;
    alert["action"] = "set_vibration_warning";
    alert["value"] = true;

    String vibData;
    serializeJson(alert, vibData);
    webSocket.sendTXT(vibData);
    Serial.println("Sent vibration alert: " + vibData);
  }

  delay(100);
}

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;
    case WStype_TEXT:
      Serial.printf("Received: %s\n", payload);
      break;
  }
}
