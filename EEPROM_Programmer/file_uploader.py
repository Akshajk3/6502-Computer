import requests

esp32_ip = "http://192.168.1.197"

response = requests.get(f"{esp32_ip}/")
print(f"Response from ESP32 (root): {response.text}")

response = requests.get(f"{esp32_ip}/data")
print(f"Response from ESP32 (/data): {response.text}")