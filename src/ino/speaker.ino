#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BluetoothA2DPSink.h>

const char* ssid = "";  // Your Wi-Fi SSID
const char* password = ""; // Your Wi-Fi password
const char* btSpeakerName = ""; // Change to your Bluetooth speaker name
const char* googleTTSAPI = ""; // Google TTS API Key

WebServer server(80);
BluetoothA2DPSink btAudio;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");

  // Start Bluetooth speaker connection
  btAudio.start(btSpeakerName);

  // Define Speak API
  server.on("/speak", HTTP_POST, []() {
    String message = server.arg("plain");
    Serial.print("Speaking: ");
    Serial.println(message);

    String googleTTSURL = "https://texttospeech.googleapis.com/v1/text:synthesize?key=YOUR_GOOGLE_API_KEY";

    // JSON for Google TTS
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["input"]["text"] = message;
    jsonDoc["voice"]["languageCode"] = "en-US";
    jsonDoc["voice"]["ssmlGender"] = "FEMALE";
    jsonDoc["audioConfig"]["audioEncoding"] = "MP3";

    String jsonData;
    serializeJson(jsonData, jsonDoc);

    HTTPClient http;
    http.begin(googleTTSURL);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode == 200) {
      Serial.println("Audio received, streaming to Bluetooth speaker...");
      String audioUrl = http.getString();
      btAudio.play(googleTTSURL.c_str()); // Play generated MP3 via Bluetooth
      server.send(200, "text/plain", "Message spoken via Bluetooth");
    } else {
      server.send(500, "text/plain", "Error generating speech");
    }
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
