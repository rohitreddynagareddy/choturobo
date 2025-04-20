#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "**";
const char* password = "***";

// Web server runs on port 80
WebServer server(80);

// Servo setup
Servo myServo;
int myServoPin = 14;

Servo baseServo;
int baseServoPin = 27;
int baseAngle = 90;

Servo shoulderServo;
int shoulderServoPin = 26;

Servo elbowServo;
int elbowServoPin = 25;  // Elbow servo on pin 25

// Moisture sensor and spray relay setup
const int moisturePin = 34;  // Analog pin for moisture sensor
const int sprayRelayPin = 32;  // Digital pin to control the spray relay

void setup() {
  Serial.begin(115200);

  // Attach servos
  myServo.attach(myServoPin);
  myServo.write(90); // Neutral position

  baseServo.attach(baseServoPin);
  baseServo.write(baseAngle);

  shoulderServo.attach(shoulderServoPin);
  shoulderServo.write(90);  // Start at neutral position

  elbowServo.attach(elbowServoPin);
  elbowServo.write(90);  // Start at neutral position

  // Initialize the spray relay pin
  pinMode(sprayRelayPin, OUTPUT);
  digitalWrite(sprayRelayPin, LOW); // Ensure spray is off initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/scissor", handleScissorRequest);
  server.on("/rotateBase", handleBaseRotation);
  server.on("/rotateShoulder", handleShoulderRotation);
  server.on("/rotateElbow", handleElbowRotation);
  server.on("/scissorSlow", handleScissorSlowRequest);
  server.on("/scissorFast", handleScissorFastRequest);
  server.on("/checkMoisture", handleMoistureCheck);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// ----------- Scissor Method --------------
void scissor(bool slow = false) {
  int delayTime = (slow) ? 100 : 10;  // Slow = 100ms delay, Fast = 10ms delay

  myServo.write(180); delay(delayTime);
  myServo.write(90);  delay(delayTime);
  myServo.write(0);   delay(delayTime);
  myServo.write(90);  delay(20);
}

// ---------- HTTP Handler ----------------
void handleScissorRequest() {
  if (!server.hasArg("repeat")) {
    server.send(400, "text/plain", "Missing 'repeat' argument.");
    return;
  }

  int repeatCount = server.arg("repeat").toInt();
  if (repeatCount <= 0) {
    server.send(400, "text/plain", "Invalid 'repeat' value.");
    return;
  }

  for (int i = 0; i < repeatCount; i++) {
    scissor();
  }

  server.send(200, "text/plain", "Scissor action completed " + String(repeatCount) + " times.");
}

void handleScissorSlowRequest() {
  if (!server.hasArg("repeat")) {
    server.send(400, "text/plain", "Missing 'repeat' argument.");
    return;
  }

  int repeatCount = server.arg("repeat").toInt();
  if (repeatCount <= 0) {
    server.send(400, "text/plain", "Invalid 'repeat' value.");
    return;
  }

  for (int i = 0; i < repeatCount; i++) {
    scissor(true);  // Slow
  }

  server.send(200, "text/plain", "Slow scissor action completed " + String(repeatCount) + " times.");
}

void handleScissorFastRequest() {
  if (!server.hasArg("repeat")) {
    server.send(400, "text/plain", "Missing 'repeat' argument.");
    return;
  }

  int repeatCount = server.arg("repeat").toInt();
  if (repeatCount <= 0) {
    server.send(400, "text/plain", "Invalid 'repeat' value.");
    return;
  }

  for (int i = 0; i < repeatCount; i++) {
    scissor(false);  // Fast
  }

  server.send(200, "text/plain", "Fast scissor action completed " + String(repeatCount) + " times.");
}

// ---------- Base Rotation ----------------
void rotateBase(int targetAngle) {
  int currentAngle = baseServo.read();
  int step = (targetAngle > currentAngle) ? 1 : -1;

  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    baseServo.write(angle);
    delay(10);
  }

  baseServo.write(targetAngle);
}

void handleBaseRotation() {
  if (!server.hasArg("degree")) {
    server.send(400, "text/plain", "Missing 'degree' argument.");
    return;
  }

  int angle = server.arg("degree").toInt();
  if (angle < 0 || angle > 180) {
    server.send(400, "text/plain", "Invalid angle. Must be between 0 and 180.");
    return;
  }

  rotateBase(angle);
  server.send(200, "text/plain", "Base rotated to " + String(angle) + " degrees.");
}

// ---------- Shoulder Rotation ----------------
void rotateShoulder(int targetAngle) {
  int currentAngle = shoulderServo.read();
  int step = (targetAngle > currentAngle) ? 1 : -1;

  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    shoulderServo.write(angle);
    delay(10);
  }

  shoulderServo.write(targetAngle);
}

void handleShoulderRotation() {
  if (!server.hasArg("degree")) {
    server.send(400, "text/plain", "Missing 'degree' argument.");
    return;
  }

  int angle = server.arg("degree").toInt();
  if (angle < 0 || angle > 180) {
    server.send(400, "text/plain", "Invalid angle. Must be between 0 and 180.");
    return;
  }

  rotateShoulder(angle);
  server.send(200, "text/plain", "Shoulder rotated to " + String(angle) + " degrees.");
}

// ---------- Elbow Rotation ----------------
void rotateElbow(int targetAngle) {
  int currentAngle = elbowServo.read();
  int step = (targetAngle > currentAngle) ? 1 : -1;

  for (int angle = currentAngle; angle != targetAngle; angle += step) {
    elbowServo.write(angle);
    delay(10);
  }

  elbowServo.write(targetAngle);
}

void handleElbowRotation() {
  if (!server.hasArg("degree")) {
    server.send(400, "text/plain", "Missing 'degree' argument.");
    return;
  }

  int angle = server.arg("degree").toInt();
  if (angle < 0 || angle > 180) {
    server.send(400, "text/plain", "Invalid angle. Must be between 0 and 180.");
    return;
  }

  rotateElbow(angle);
  server.send(200, "text/plain", "Elbow rotated to " + String(angle) + " degrees.");
}

// ---------- Moisture Check and Spray Method --------------
void checkMoistureAndSpray() {
  int moistureValue = analogRead(moisturePin);  // Read the moisture sensor value
  Serial.println("Moisture Level: " + String(moistureValue));

  if (moistureValue < 1000) {  // Threshold for dry soil (adjust as needed)
    Serial.println("hair  is dry, activating spray.");
    digitalWrite(sprayRelayPin, HIGH);  // Turn on spray
    delay(5000);  // Spray for 5 seconds (adjust as needed)
    digitalWrite(sprayRelayPin, LOW);   // Turn off spray
    Serial.println("hair spray activated and turned off.");
  } else {
    Serial.println("hari is moist, no spray needed.");
  }
}

void handleMoistureCheck() {
  checkMoistureAndSpray();
  server.send(200, "text/plain", "Moisture check completed.");
}
