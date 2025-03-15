#include <WiFi.h>
#include <WebServer.h>
#include <Servo.h>
#include <Motor.h>
#include <Led.h>

const char* ssid = "mywifi"; //
const char* password = "mypass"; //
WebServer server(80);  //

// Define pins
const int ledPin = 13;    // LED Pin
const int motorPinPWM = 6; // Motor PWM Pin
const int motorPinDir = 7; // Motor Direction Pin
const int servoPin = 9;   // Servo Pin
const int fanPin = 4;     // Fan Pin

// Create servo and motor objects
Servo myServo;
Motor myMotor(motorPinPWM, motorPinDir); // PWM and direction pins for motor
Led myFan(fanPin);  // Using LED class to control the fan (on/off)

void setup() {
  Serial.begin(115200);  // Start serial communication
  WiFi.begin(ssid, password);  // Start Wi-Fi connection

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());  // Print ESP32's local IP address

  // Define routes for different actions
  server.on("/blinkLED", HTTP_GET, []() {
    int time = 1000; // Default blink time in milliseconds
    Serial.println("LED blink command received.");
    blinkLED(time); // Blink LED for the specified time
    server.send(200, "text/plain", "LED is blinking!");
  });

  server.on("/startMotor", HTTP_POST, []() {
    String body = server.arg("plain");  // Get POST body
    int speed = body.toInt();  // Convert the body to an integer for motor speed
    Serial.print("Start motor with speed: ");
    Serial.println(speed);
    startMotor(speed);  // Start motor with the received speed
    server.send(200, "text/plain", "Motor started!");
  });

  server.on("/moveServo", HTTP_GET, []() {
    String angleStr = server.arg("angle");  // Get 'angle' from the query parameters
    int angle = angleStr.toInt();  // Convert it to an integer
    Serial.print("Move servo to angle: ");
    Serial.println(angle);
    moveServo(angle);  // Move servo to the specified angle
    server.send(200, "text/plain", "Servo moved!");
  });

  server.on("/controlFan", HTTP_POST, []() {
    String body = server.arg("plain");  // Get POST body
    bool state = (body == "on");  // Convert to boolean (on/off)
    controlFan(state);  // Turn fan on or off based on state
    server.send(200, "text/plain", "Fan control updated!");
  });

  // Start the HTTP server
  server.begin();
}

void loop() {
  server.handleClient();  // Handle incoming client requests
}

// Define helper methods for hardware control

void blinkLED(int time) {
  // Blink the LED for the specified time
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // Turn on LED
  delay(time);                 // Wait for 'time' milliseconds
  digitalWrite(ledPin, LOW);  // Turn off LED
}

void startMotor(int speed) {
  // Start motor with a specified speed
  pinMode(motorPinPWM, OUTPUT);
  pinMode(motorPinDir, OUTPUT);

  analogWrite(motorPinPWM, speed);  // Set motor speed using PWM
  digitalWrite(motorPinDir, HIGH);  // Set motor direction to forward
}

void moveServo(int angle) {
  // Move the servo to the specified angle
  myServo.attach(servoPin);  // Attach servo to the pin
  myServo.write(angle);      // Move servo to the angle
  delay(500);                // Wait for the servo to reach the position
  myServo.detach();          // Detach the servo
}

void controlFan(bool state) {
  // Control the fan (LED class is used to turn on/off)
  pinMode(fanPin, OUTPUT);
  if (state) {
    digitalWrite(fanPin, HIGH);  // Turn fan on
  } else {
    digitalWrite(fanPin, LOW);   // Turn fan off
  }
}
