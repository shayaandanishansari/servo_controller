/*
  arduino-cli setup commands (run once):
    arduino-cli core install esp32:esp32
    arduino-cli lib install "ESP32Servo"

  Find COM port:
    arduino-cli board list

  Compile:
    arduino-cli compile --fqbn esp32:esp32:esp32 servo_test

  Upload (replace COMx with your port):
    arduino-cli upload -p COMx --fqbn esp32:esp32:esp32 servo_test
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <Preferences.h>

const char* SSID     = "PTCL FIBER 4G F1";
const char* PASSWORD = "34985430";

IPAddress staticIP(192, 168, 1, 150);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#define SERVO_PIN 13
#define STEP      10

Servo servo;
WebServer server(80);
Preferences prefs;
int pos;
bool servoAttached = false;

void moveServo(int newPos) {
  if (!servoAttached) {
    servo.attach(SERVO_PIN);
    servoAttached = true;
  }
  pos = newPos;
  servo.write(pos);
  prefs.putInt("pos", pos);
}

void homeServo() {
  // First-ever command: attach() snaps servo to 90 (ESP32Servo default pulse),
  // so record 90 as our reference without applying a step.
  servo.attach(SERVO_PIN);
  servoAttached = true;
  pos = 90;
  prefs.putInt("pos", pos);
  Serial.printf("First boot: servo homed to %d deg\n", pos);
  server.send(200, "text/plain", String(pos));
}

void handleLeft() {
  if (pos < 0) { homeServo(); return; }
  moveServo(constrain(pos - STEP, 0, 180));
  Serial.printf("GET /left  -> %d deg\n", pos);
  server.send(200, "text/plain", String(pos));
}

void handleRight() {
  if (pos < 0) { homeServo(); return; }
  moveServo(constrain(pos + STEP, 0, 180));
  Serial.printf("GET /right -> %d deg\n", pos);
  server.send(200, "text/plain", String(pos));
}

void setup() {
  Serial.begin(115200);

  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected. IP: %s\n", WiFi.localIP().toString().c_str());

  prefs.begin("servo", false);
  pos = prefs.getInt("pos", -1);  // -1 means no saved position yet
  Serial.printf("Restored position: %d deg\n", pos);

  server.on("/left",  HTTP_GET, handleLeft);
  server.on("/right", HTTP_GET, handleRight);
  server.begin();
  Serial.println("HTTP server started on port 80");
}

void loop() {
  server.handleClient();
}
