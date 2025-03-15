#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <HTTPClient.h>
#include <BluetoothA2DPSink.h>

const char* ssid = ""; //
const char* password = ""; //
const char* btSpeakerName = ""; //
const char* openaiApiKey = ""; //

WebServer server(80);
BluetoothA2DPSink btAudio;

const int ledPin = 13;
const int motorRightPWM = 6;
const int motorRightDir = 7;
const int motorLeftPWM = 8;
const int motorLeftDir = 9;
const int servoPin = 9;
const int servoPin = 9;
const int fanPin = 4;
const int moisturePin = A0;
const int distanceTrigPin = 5;
const int distanceEchoPin = 6;
const int tempSensorPin = A0;

Servo myServo;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT);
  pinMode(motorRightPWM, OUTPUT);
  pinMode(motorRightDir, OUTPUT);
  pinMode(motorLeftPWM, OUTPUT);
  pinMode(motorLeftDir, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(distanceTrigPin, OUTPUT);
  pinMode(distanceEchoPin, INPUT);
  pinMode(servoPin, OUTPUT);

  // Start Bluetooth speaker connection
  btAudio.start(btSpeakerName);

  // API Endpoints
  server.on("/status", HTTP_GET, []() {
    StaticJsonDocument<200> doc;
    doc["led"] = digitalRead(ledPin);
    doc["RightMotor"] = analogRead(motorRightPWM);
    doc["LeftMotor"] = analogRead(motorLeftPWM);
    doc["Servo"] = myServo.read();
    doc["Moisture"] = analogRead(moisturePin);
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
    if (doc.containsKey("motorRight")) {
      int speed = doc["motorRight"].as<int>();
      analogWrite(motorRightPWM, abs(speed));
      digitalWrite(motorRightDir, speed > 0 ? HIGH : LOW);
    }
    if (doc.containsKey("motorLeft")) {
      int speed = doc["leftMotor"].as<int>();
      analogWrite(motorLeftPWM, abs(speed));
      digitalWrite(motorLeftDir, speed > 0 ? HIGH : LOW);
    }
    if (doc.containsKey("servo")) {
      myServo.attach(servoPin);
      myServo.write(doc["servo"].as<int>());
      delay(500);
      myServo.detach();
    }
    if (doc.containsKey("fan")) digitalWrite(fanPin, doc["fan"].as<bool>() ? HIGH : LOW);

    server.send(200, "text/plain", "Control updated");
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
    StaticJsonDocument<200> doc;
    deserializeJson(doc, server.arg("plain"));
    String message = doc["message"].as<String>();
    Serial.print("Speaking: ");
    Serial.println(message);

    HTTPClient http;
    http.begin("https://api.openai.com/v1/audio/speech");
    http.addHeader("Authorization", "Bearer " + String(openaiApiKey));
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["model"] = "tts-1";
    jsonDoc["input"] = message;
    jsonDoc["voice"] = "alloy";
    String requestBody;
    serializeJson(jsonDoc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode == 200) {
      String audioURL = http.getString();
      Serial.print("Audio URL: ");
      Serial.println(audioURL);
      btAudio.play(audioURL.c_str()); // Play audio on Bluetooth speaker
      server.send(200, "text/plain", "Message spoken via Bluetooth");
    } else {
      Serial.println("Error generating speech");
      server.send(500, "text/plain", "Failed to convert text to speech");
    }

    http.end();
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
  analogWrite(motorRightPWM, 0);
  analogWrite(motorLeftPWM, 255);
  digitalWrite(motorRightDir, LOW);
  digitalWrite(motorLeftDir, HIGH);
}

float getDistance() {
  // Implement ultrasonic sensor logic
}

float getTemperature() {
  // Implement temperature sensor logic
}
