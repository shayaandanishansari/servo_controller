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

Servo panServo;
Servo tiltServo;
BLECharacteristic* pTxChar;

int panPos  = 0;
int tiltPos = 0;

class RxCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) override {
        String cmd = pChar->getValue();
        if (cmd.length() == 0) return;
        cmd.trim();

        String feedback;

        if (cmd == "right") {
            for (int i = 0; i < STEP; i++){
                panPos += 1;
                panServo.write( panPos);
                delay(15);
            }
            feedback = "right:moved";
        }
        else if (cmd == "left") {
            for (int i = 0; i < STEP; i++) {
                panPos -= 1;
                panServo.write(panPos);
                delay(15);
            }
            feedback = "left:moved";
        }
        else if (cmd == "up") {
            for (int i = 0; i < STEP; i++) {
                tiltPos -= 1;
                tiltServo.write(tiltPos);
                delay(15);
            }
            feedback = "up:moved";
        }
        else if (cmd == "down") {
            for (int i = 0; i < STEP; i++) {
                tiltPos += 1;
                tiltServo.write(tiltPos);
                delay(15);
            }
            feedback = "down:moved";
        }
        else {
            return;
        }

        pTxChar->setValue(feedback.c_str());
        pTxChar->notify();
        Serial.println(feedback);
    }
};

void setupServos() {
    panServo.attach(PAN_PIN);
    tiltServo.attach(TILT_PIN);
}

void setupBLE() {
    BLEDevice::init("PanTiltAgent");
    BLEServer*  pServer  = BLEDevice::createServer();
    BLEService* pService = pServer->createService(SERVICE_UUID);

    pTxChar = pService->createCharacteristic(
        CHAR_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY
    );
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

void setup() {
    Serial.begin(115200);
    setupServos();
    setupBLE();
}

void loop() {
    delay(10);
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
