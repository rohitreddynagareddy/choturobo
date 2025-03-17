#include <WiFi.h>
#include <WebServer.h>
#include <Stepper.h>

const char* ssid = "user";    // Your WiFi SSID
const char* password = "pass"; // Your WiFi password
WebServer server(80);  // Web server instance on port 80

// Stepper motor pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

const int stepsPerRevolution = 2048;  // Number of steps per revolution

// Initialize the stepper library
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {

  Serial.begin(115200);  // Start serial communication
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);  // Start Wi-Fi connection


  while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());  // Print ESP32's local IP address

  // Set the motor speed to 5 RPM
  myStepper.setSpeed(5);

  // Define routes for moving the motor in both directions
  server.on("/clockwise", HTTP_GET, []() {
    Serial.println("Moving motor clockwise");
    myStepper.step(stepsPerRevolution);  // Move one revolution clockwise
    server.send(200, "text/plain", "Motor moved clockwise");
  });

  server.on("/counterclockwise", HTTP_GET, []() {
    Serial.println("Moving motor counterclockwise");
    myStepper.step(-stepsPerRevolution);  // Move one revolution counterclockwise
    server.send(200, "text/plain", "Motor moved counterclockwise");
  });

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();  // Handle incoming client requests
}
