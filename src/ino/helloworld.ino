#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);  //
const char* ssid = ""; // 🔹 Replace with your Wi-Fi SSID
const char* password = ""; // 🔹 Replace with your Wi-Fi Password

void setup() {
  Serial.begin(115200);  // Start serial communication
  WiFi.begin(ssid, password); // Start Wi-Fi connection

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP()); // Print ESP32's local IP address
  server.on("/sayHello", HTTP_GET, []() {

      Serial.println("Who said Hello");

      server.send(200, "text/plain", "Hello How are you!");
    });
    server.begin();
}

void loop() {
 server.handleClient();  // Handle incoming client requests
}
