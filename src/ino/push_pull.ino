#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

Servo fanServo;
// Motor control pins
#define IN1 25    // Motor 1 Direction Pin 1
#define IN2 26    // Motor 1 Direction Pin 2
#define IN3 27    // Motor 2 Direction Pin 1
#define IN4 14    // Motor 2 Direction Pin 2
// WiFi credentials
const char* ssid = "";
const char* password = "";

// Create WebServer object on port 80
WebServer server(80);
void setup() {
  Serial.begin(115200);
   fanServo.attach(13);  // GPIO D35
    fanServo.write(0);

  // Set motor direction pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  // Define REST API routes
  server.on("/moveForward", HTTP_GET, handleMoveForward);
  server.on("/moveBackward", HTTP_GET, handleMoveBackward);
  server.on("/moveRandom", HTTP_GET, handleMoveRandom);
  server.on("/pushPull", HTTP_GET, pushPull);
  server.on("/stop", HTTP_GET, handleStopMotors);
  server.on("/startFan", HTTP_GET, handleStartFan);
  server.on("/stopFan", HTTP_GET, handleStopFan);
  server.on("/namaste", HTTP_GET, namaste);
  // Start server
  server.begin();
  Serial.println("Car ready to move!");
}
void handleStartFan() {
  for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    fanServo.write(posDegrees);

    delay(20);
  }

  for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    fanServo.write(posDegrees);
    Serial.println(posDegrees);
    delay(20);
  }
  server.send(200, "text/plain", "startedfan.");
}

void handleStopFan() {
  Serial.println("Fan stopped");
  fanServo.write(90);   // Neutral (for continuous rotation servos) or hold position
}

void loop() {
  // Handle incoming requests
  server.handleClient();
}
void handleStopMotors() {
  stopMotors();
  handleStopFan();
  server.send(200, "text/plain", "Motors stopped.");
}
void handleMoveRandom() {
  Serial.println("Moving randomly for 6 seconds...");

  unsigned long startTime = millis();

  while (millis() - startTime < 6000) {   // Move for 6 seconds
    int direction = random(1, 5);         // Random number between 1-4

    switch (direction) {
      case 1:
        Serial.println("Moving forward...");
        moveForward();
        break;
      case 2:
        Serial.println("Moving backward...");
        moveBackward();
        break;
      case 3:
        Serial.println("Turning left...");
        turnLeft();
        break;
      case 4:
        Serial.println("Turning right...");
        turnRight();
        break;
    }
    delay(500);   // Move in the random direction for 500ms
  }

  // Stop motors after random movement
  stopMotors();
  server.send(200, "text/plain", "Moved randomly for 6 seconds.");
}
void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void namaste() {
  if (server.hasArg("name")) {
    String name = server.arg("name");

    // Generate a random number between 1 and 1000
    int randomNumber = random(1, 1001);

    // Send response with name and random number
    String message = "Namaste to " + name + " from ChotuRobo. Random number: " + String(randomNumber);
    server.send(200, "text/plain", message);
  } else {
    server.send(400, "text/plain", "Name parameter is missing.");
  }
}

void pushPull() {
  // Move forward for 5 seconds (continuously)
  Serial.println("Moving forward...");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {   // Run for 5 seconds
    moveForward();
  }

  // Move backward for 5 seconds (continuously)
  Serial.println("Moving backward...");
  startTime = millis();
  while (millis() - startTime < 5000) {   // Run for 5 seconds
    moveBackward();
  }
}

void handleMoveForward() {
  if (server.hasArg("steps")) {
    int steps = server.arg("steps").toInt();
   unsigned long startTime = millis();
  while (millis() - startTime < steps *1000) {
    moveForward();
  }
    server.send(200, "text/plain", "Moving forward with steps " + String(steps));
  } else {
    server.send(400, "text/plain", "steps parameter is missing.");
  }
}

void handleMoveBackward() {
  if (server.hasArg("steps")) {
    int steps = server.arg("steps").toInt();
    unsigned long startTime = millis();
  while (millis() - startTime < steps *1000) {
    moveBackward();
  }
    server.send(200, "text/plain", "Moving backward with steps " + String(steps));
  } else {
    server.send(400, "text/plain", "steps parameter is missing.");
  }
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Function to move the car forward
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Function to move the car backward
void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
