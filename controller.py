import requests
from pynput import keyboard

TARGET_IP = "192.168.1.150"
BASE_URL = f"http://{TARGET_IP}"


def send(endpoint):
    url = f"{BASE_URL}{endpoint}"
    print(f"-> {url}", end="  ", flush=True)
    try:
        r = requests.get(url, timeout=3)
        print(f"[{r.status_code}] pos={r.text.strip()} deg")
    except requests.exceptions.RequestException as e:
        print(f"ERROR: {e}")


def on_press(key):
    if key == keyboard.Key.left:
        send("/left")
    elif key == keyboard.Key.right:
        send("/right")
    elif key == keyboard.KeyCode.from_char('q'):
        return False
    return None


print(f"Servo controller targeting {BASE_URL}")
print("Left/Right arrows to move | Q to quit\n")

with keyboard.Listener(on_press=on_press) as listener:
    listener.join()

print("Exiting.")
