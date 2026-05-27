import asyncio
from pynput import keyboard
from bleak import BleakClient, BleakScanner

DEVICE_NAME  = "PanTiltAgent"
NUS_RX_UUID  = "6E400002-B5B3-F393-E0A9-E50E24DCCA9E"
NUS_TX_UUID  = "6E400003-B5B3-F393-E0A9-E50E24DCCA9E"

KEY_MAP = {
    keyboard.Key.right: "right",
    keyboard.Key.left:  "left",
    keyboard.Key.up:    "up",
    keyboard.Key.down:  "down",
}

def on_feedback(sender, data):
    print(f"[ESP32] {data.decode()}")

async def connect():
    print(f"Scanning for '{DEVICE_NAME}'...")
    device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=10.0)
    if not device:
        print(f"[ERROR] '{DEVICE_NAME}' not found. Is the ESP32 powered and advertising?")
        exit(1)
    print(f"Connected to {device.name} ({device.address})")
    return device

async def listen(client):
    await client.start_notify(NUS_TX_UUID, on_feedback)

    loop     = asyncio.get_event_loop()
    stop_evt = asyncio.Event()

    def on_press(key):
        cmd = KEY_MAP.get(key)
        if cmd:
            asyncio.run_coroutine_threadsafe(
                client.write_gatt_char(NUS_RX_UUID, cmd.encode()),
                loop
            )
        elif key == keyboard.KeyCode.from_char("q"):
            loop.call_soon_threadsafe(stop_evt.set)
            return False

    with keyboard.Listener(on_press=on_press):
        await stop_evt.wait()

    await client.stop_notify(NUS_TX_UUID)
    print("Disconnected.")

async def main():
    device = await connect()
    async with BleakClient(device) as client:
        await listen(client)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Exiting.")

"""
import pynput
import bluetooth connection library

1 Connect with ESP32 via BlueTooth

2 Handle Failure + Log

3 Loop for:
    .1 Right Arrow Key -> Send right move to ESP
    .2 Left Arrow Key -> Send Left move to ESP
    .3 Up Arrow Key -> Send Up move to ESP
    .4 Down Arrow Key -> Send Down move to ESP

4 In Loop. Handle failure + Log

5  Catch Blue tooth connection failure, and exit app.
    ELSE keep running forever unless Q pressed or CTRL+C (Handled by default terminal likely)
"""
