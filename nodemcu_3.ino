#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "vivo_T1";
const char* password = "22222222";
const char* server = "api.thingspeak.com";
const String apiKey = "JWSPABYH1818VU2A";

WiFiClient client;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  pinMode(D1, OUTPUT); // Blue LED pin for data transmission status
  digitalWrite(D1, LOW); // Ensure LED is off initially

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");
  digitalWrite(D1, HIGH); // Turn on Green LED when connected (can be the existing Wi-Fi LED)
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    sendDataToThingSpeak(data);
  }
}

void sendDataToThingSpeak(String data) {

  Serial.println("Data to be sent: ");
  Serial.println(data);
  
  if (client.connect(server, 80)) {

    // testing
    Serial.println("Sending HTTP request:");
    Serial.print("POST /update HTTP/1.1\n");
    Serial.print("Host: api.thingspeak.com\n");
    Serial.print("Connection: close\n");
    Serial.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    Serial.print("Content-Type: application/x-www-form-urlencoded\n");
    Serial.print("Content-Length: ");
    Serial.print(data.length());
    Serial.print("\n\n");
    Serial.print(data);
    Serial.print("\n\n");    

    // send the actual HTTP request
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data);

    // Wait for server to respond
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 10000) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);  // Print server response for debugging
      }
    }
    
    client.stop(); // stop client connection
    Serial.println("Data sent to ThingSpeak");

    // Indicate successful data transmission
    digitalWrite(D1, HIGH);
    delay(100); // Briefly turn on the LED
    digitalWrite(D1, LOW);
  } else {
    Serial.println("Failed to connect to ThingSpeak");
    // Keep the LED off if data transmission fails
    digitalWrite(D1, LOW);
  }
}
