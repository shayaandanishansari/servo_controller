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