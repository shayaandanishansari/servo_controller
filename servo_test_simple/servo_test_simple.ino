// Compile: arduino-cli compile --fqbn esp32:esp32:esp32 servo_test_simple
// Upload:  arduino-cli upload -p COM<X> --fqbn esp32:esp32:esp32 servo_test_simple

#include <ESP32Servo.h>

Servo servo;
const int SERVO_PIN = 13;

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  Serial.println("Servo test started");
}

void loop() {
  // Sweep 0 -> 180
  for (int pos = 0; pos <= 180; pos++) {
    servo.write(pos);
    delay(15);
  }
  delay(1000);

  // Sweep 180 -> 0
  for (int pos = 180; pos >= 0; pos--) {
    servo.write(pos);
    delay(15);
  }
  delay(1000);
}
