import requests

esp32_ip = "http://192.168.1.197"
upload_URL = f"{esp32_ip}/upload"
file_name = "a.out"

def upload_file():
    with open(file_name, "rb") as f:
        data = f.read()

        print(f"Uploading {len(data)} bytes to {esp32_ip}")

        response = requests.post(upload_URL, files={"file" : data})

        print("Reponse: ", response.text)

upload_file()