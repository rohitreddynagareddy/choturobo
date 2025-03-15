#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>

const char* ssid = "mywifi"; //
const char* password = "mypass"; //
WebServer server(80);

// Define pins
const int ledPin = 13;
const int motorRightPWM = 6;
const int motorRightDir = 7;
const int motorLeftPWM = 8;
const int motorLeftDir = 9;
const int servoPin = 10;
const int fanPin = 4;
const int moistureSensorPin = A0;
const int distanceTrigPin = 5;
const int distanceEchoPin = 6;
const int tempSensorPin = A1;

// Servo setup
Servo myServo;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(moistureSensorPin, INPUT);
  pinMode(distanceTrigPin, OUTPUT);
  pinMode(distanceEchoPin, INPUT);
  pinMode(tempSensorPin, INPUT);

  // API Endpoints
  server.on("/status", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["LED"] = digitalRead(ledPin);
    doc["RightMotor"] = analogRead(motorRightPWM);
    doc["LeftMotor"] = analogRead(motorLeftPWM);
    doc["Servo"] = myServo.read();
    doc["Fan"] = digitalRead(fanPin);
    doc["Moisture"] = analogRead(moistureSensorPin);
    doc["Distance"] = getDistance();
    doc["Temperature"] = getTemperature();
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  server.on("/control", HTTP_POST, []() {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, server.arg("plain"));

    if (doc.containsKey("led")) digitalWrite(ledPin, doc["led"].as<bool>() ? HIGH : LOW);
    if (doc.containsKey("rightMotor")) {
      int speed = doc["rightMotor"].as<int>();
      analogWrite(motorRightPWM, speed);
      digitalWrite(motorRightDir, HIGH);
    }
    if (doc.containsKey("leftMotor")) {
      int speed = doc["leftMotor"].as<int>();
      analogWrite(motorLeftPWM, speed);
      digitalWrite(motorLeftDir, HIGH);
    }
    if (doc.containsKey("servo")) {
      myServo.attach(servoPin);
      myServo.write(doc["servo"].as<int>());
      delay(500);
      myServo.detach();
    }
    if (doc.containsKey("fan")) digitalWrite(fanPin, doc["fan"].as<bool>() ? HIGH : LOW);

    server.send(200, "text/plain", "Success");
  });

  server.on("/move", HTTP_POST, []() {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, server.arg("plain"));
    if (doc.containsKey("direction")) {
      String direction = doc["direction"].as<String>();
      if (direction == "forward") moveForward();
      else if (direction == "backward") moveBackward();
      else if (direction == "left") rotateLeft();
      else if (direction == "right") rotateRight();
    }
    server.send(200, "text/plain", "Movement executed");
  });

  server.on("/speak", HTTP_POST, []() {
    String message = server.arg("plain");
    Serial.print("Speaking: ");
    Serial.println(message);
    server.send(200, "text/plain", "Message spoken");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}

void moveForward() {
  analogWrite(motorRightPWM, 255);
  analogWrite(motorLeftPWM, 255);
  digitalWrite(motorRightDir, HIGH);
  digitalWrite(motorLeftDir, HIGH);
}

void moveBackward() {
  analogWrite(motorRightPWM, 255);
  analogWrite(motorLeftPWM, 255);
  digitalWrite(motorRightDir, LOW);
  digitalWrite(motorLeftDir, LOW);
}

void rotateLeft() {
  analogWrite(motorRightPWM, 255);
  analogWrite(motorLeftPWM, 0);
  digitalWrite(motorRightDir, HIGH);
  digitalWrite(motorLeftDir, LOW);
}

void rotateRight() {
  analogWrite(motorRightPWM, 255);
  analogWrite(motorLeftPWM, 0);
  digitalWrite(motorRightDir, LOW);
  digitalWrite(motorLeftDir, HIGH);
}

long getDistance() {
  digitalWrite(distanceTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(distanceTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(distanceTrigPin, LOW);
  long duration = pulseIn(distanceEchoPin, HIGH);
  return (duration * 0.034 / 2);
}
