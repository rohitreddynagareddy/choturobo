#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "**";
const char* password = "**";

// Web server running on port 80
WebServer server(80);

// Servo objects
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripperServo;
int currentBase = 90;
int currentShoulder = 90;
int currentElbow = 90;
int currentGripper = 90;
// Function prototypes
void moveBase();
void moveShoulder();
void moveElbow();
void moveGripper();

void setup() {
  Serial.begin(115200);

  // Attach servo pins
  baseServo.attach(14);
  shoulderServo.attach(27);
  elbowServo.attach(16);
  gripperServo.attach(17);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  // Set up server routes
  server.on("/moveBase", HTTP_GET, moveBase);
  server.on("/moveShoulder", HTTP_GET, moveShoulder);
  server.on("/moveElbow", HTTP_GET, moveElbow);
  server.on("/moveGripper", HTTP_GET, moveGripper);
  server.on("/namaste", HTTP_GET, namaste);
  server.begin();
}

// Common function to handle servo movement
int& getCurrentPositionRef(const String& name) {
  if (name == "Base") return currentBase;
  else if (name == "Shoulder") return currentShoulder;
  else if (name == "Elbow") return currentElbow;
  else return currentGripper;
}

void handleServoMove(Servo &servo, const String& name) {
  if (server.hasArg("degrees")) {
    int target = server.arg("degrees").toInt();
    if (target >= 0 && target <= 180) {
      int& current = getCurrentPositionRef(name);

      int step = (target > current) ? 1 : -1;
      for (int pos = current; pos != target; pos += step) {
        servo.write(pos);
        delay(10);  // Adjust delay for smoother/faster movement
      }
      servo.write(target);  // Final adjustment
      current = target;     // Update current position

      server.send(200, "text/plain", name + " moved to " + String(target) + " degrees (smooth)");
    } else {
      server.send(400, "text/plain", "Degrees must be between 0 and 180");
    }
  } else {
    server.send(400, "text/plain", "Missing 'degrees' parameter");
  }
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

void moveBase() {
  handleServoMove(baseServo, "Base");
}

void moveShoulder() {
  handleServoMove(shoulderServo, "Shoulder");
}

void moveElbow() {
  handleServoMove(elbowServo, "Elbow");
}

void moveGripper() {
  handleServoMove(gripperServo, "Gripper");
}

void loop() {
  server.handleClient();
}
