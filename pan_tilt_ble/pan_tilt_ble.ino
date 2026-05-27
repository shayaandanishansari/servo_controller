#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32Servo.h>

#define SERVICE_UUID   "6E400001-B5B3-F393-E0A9-E50E24DCCA9E"
#define CHAR_UUID_RX   "6E400002-B5B3-F393-E0A9-E50E24DCCA9E"
#define CHAR_UUID_TX   "6E400003-B5B3-F393-E0A9-E50E24DCCA9E"

#define PAN_PIN   13
#define TILT_PIN  15
#define STEP      10
#define PAN_MIN   0
#define PAN_MAX   100
#define TILT_MIN  0
#define TILT_MAX  50

Servo panServo;
Servo tiltServo;
BLECharacteristic* pTxChar;

int panPos, tiltPos;
int panTarget, tiltTarget;

class RxCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) override {
        std::string raw = pChar->getValue();
        if (raw.length() == 0) return;

        String cmd = String(raw.c_str());
        cmd.trim();

        int newPanTarget  = panTarget;
        int newTiltTarget = tiltTarget;

        if      (cmd == "right") newPanTarget  = panTarget  + STEP;
        else if (cmd == "left")  newPanTarget  = panTarget  - STEP;
        else if (cmd == "up")    newTiltTarget = tiltTarget - STEP;
        else if (cmd == "down")  newTiltTarget = tiltTarget + STEP;

        newPanTarget  = constrain(newPanTarget,  PAN_MIN,  PAN_MAX);
        newTiltTarget = constrain(newTiltTarget, TILT_MIN, TILT_MAX);

        String feedback;
        if (newPanTarget != panTarget || newTiltTarget != tiltTarget) {
            panTarget  = newPanTarget;
            tiltTarget = newTiltTarget;
            feedback   = cmd + ":moved";
        } else {
            feedback = cmd + ":limit";
        }

        pTxChar->setValue(feedback.c_str());
        pTxChar->notify();
        Serial.println(feedback);
    }
};

void setup() {
    Serial.begin(115200);

    panServo.attach(PAN_PIN);
    tiltServo.attach(TILT_PIN);
    // No write() — servos stay exactly where they are physically
    panPos = panTarget = panServo.read();
    tiltPos = tiltTarget = tiltServo.read();

    BLEDevice::init("PanTiltAgent");
    BLEServer*  pServer  = BLEDevice::createServer();
    BLEService* pService = pServer->createService(SERVICE_UUID);

    pTxChar = pService->createCharacteristic(CHAR_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxChar->addDescriptor(new BLE2902());

    BLECharacteristic* pRxChar = pService->createCharacteristic(
        CHAR_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    pRxChar->setCallbacks(new RxCallbacks());

    pService->start();
    BLEDevice::getAdvertising()->start();
    Serial.println("BLE NUS running. Waiting for connection...");
}

void loop() {
    if (panPos < panTarget) {
        panServo.write(++panPos);
        delay(15);
    } else if (panPos > panTarget) {
        panServo.write(--panPos);
        delay(15);
    }

    if (tiltPos < tiltTarget) {
        tiltServo.write(++tiltPos);
        delay(15);
    } else if (tiltPos > tiltTarget) {
        tiltServo.write(--tiltPos);
        delay(15);
    }
}

/*
include necessary libs

1. Start BlueTooth and connect with pan_tilt_ble_controller.py process
    Send connection successful feedback

2. Attach Servos (Servos should begin exactly where they are not move around anywhere)
    * DOCS: https://docs.arduino.cc/libraries/servo/
    PanServo 1 PIN 13
    TiltServo 2 PIN 15
    Send Servo connection feedback

3. In loop Read BLE MESSAGES:
    right -> PanServo degree +10 clockwise
    left -> PanServo degree -10 counter-clockwise
    up -> TiltServo degree -10 clockwise
    down -> TiltServo degree +10 counterclockwise

4. Send feedback of movement back to command process

CONTINUE FOREVER
 */
