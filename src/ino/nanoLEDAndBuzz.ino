// Pin where the LED is connected
const int ledPin = 10;
// Pin where the buzzer is connected
const int buzzerPin = 12;

// Setup function runs once when you reset your Arduino
void setup() {
  // Initialize the digital pins as output.
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

// Loop function runs repeatedly
void loop() {
  // Turn the LED on (HIGH is the voltage level)
  digitalWrite(ledPin, HIGH);
  // Wait for 1 second (1000 milliseconds)
  delay(1000);

  // Turn the LED off by making the voltage LOW
  digitalWrite(ledPin, LOW);
  // Wait for 1 second
  delay(1000);

  // Turn the buzzer on
  digitalWrite(buzzerPin, HIGH);
  // Wait for 4 seconds (4000 milliseconds)
  delay(4000);

  // Turn the buzzer off
  digitalWrite(buzzerPin, LOW);
  // Wait for 1 second
  delay(1000);
}
