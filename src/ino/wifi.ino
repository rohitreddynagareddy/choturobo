#include <WiFi.h>

const char* ssid = "";       // 🔹 Replace with your Wi-Fi SSID
const char* password = "";   // 🔹 Replace with your Wi-Fi Password

void setup() {
    Serial.begin(115200);  // Start serial communication

    WiFi.begin(ssid, password);  // Start Wi-Fi connection
    Serial.print("Connecting to Wi-Fi");

    // Wait for Wi-Fi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());  // Print ESP32's local IP address
}

void loop() {
    // Your main code here
}
