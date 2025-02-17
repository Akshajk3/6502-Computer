#include <WiFi.h>
#include <WebServer.h>

const char ADDR[] = {32, 33, 25, 26, 27, 14, 12, 13, 23, 22, 1, 3, 21, 19, 18, 5}; 
const char DATA[] = {17, 16, 4};


#define WE 15
#define OE 0
#define CE 2

const char* ssid = "Kanumuri-TP";
const char* password = "9848016272";

WebServer server(80);

void setup() {
  Serial.begin(115200);

  /*  Initialize the EERPOM and setup the address and data pins  */
  for (int i = 0; i < sizeof(ADDR) / sizeof(ADDR[0]); i++) {
    pinMode(ADDR[i], OUTPUT);
  }

  for (int i = 0; i < sizeof(DATA) / sizeof(DATA[0]); i++) {
    pinMode(DATA[i], OUTPUT);
  }

  pinMode(WE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(CE, OUTPUT);

  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(CE, LOW);

  delay(1000);

  /*  Server code to initlaize wifi communciation with computer to upload code binary   */
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }

  // Serial.println("Connected to WiFi");
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());

  // server.on("/", HTTP_GET, [] () {
  //   server.send(200, "text/plain", "Hello from ESP32!");
  // });

  // server.on("/data", HTTP_GET, [] () {
  //   server.send(200, "application/json", "{\"message\": \"Data received from ESP32\"}");
  // });

  // server.begin();
}

void loop() {
  //Server Code
  // server.handleClient();

  writeEEPROM(0x0000, 0x55);
  writeEEPROM(0x0001, 0xAA);

  byte data1 = readEEPROM(0x0000);
  byte data2 = readEEPROM(0x0001);

  Serial.print("Data at address 0x0000: ");
  Serial.println(data1, HEX);

  Serial.print("Data at address 0x0001: ");
  Serial.println(data2, HEX);

  delay(2000);
}

void writeEEPROM(uint16_t addr, byte data) {
  for (int i = 0; i < sizeof(ADDR) / sizeof(ADDR[0]); i++) {
    digitalWrite(ADDR[i], (addr >> i) & 0x01);
  }

  for (int i = 0; i < sizeof(DATA) / sizeof(DATA[0]); i++) {
    digitalWrite(DATA[i], (data >> i) & 0x01);
  }

  digitalWrite(CE, LOW);
  digitalWrite(WE, LOW);
  delay(1);
  digitalWrite(WE, HIGH);

  digitalWrite(CE, HIGH);
}

byte readEEPROM(uint16_t addr) {
  byte data = 0;

  for (int i = 0; i < 16; i++) {
    digitalWrite(ADDR[i], (addr >> i) & 0x01);
  }

  for (int i = 0; i < 8; i++) {
    data |= (digitalRead(DATA[i]) << i);
  }

  digitalWrite(OE, HIGH);
  digitalWrite(CE, HIGH);

  for (int i = 0; i < 8; i++) {
    pinMode(DATA[i], OUTPUT);
  }

  return data;
}