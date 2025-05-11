#define WE A0
#define OE A1

const char DATA[] = {13, 12, 11, 10, 9, 8, 7, 6};
const char ADDR[] = {5, 4, 3, 2, A5, A4, A3};

const unsigned char EEPROM_ADDR = 0x00;
char lastVal = 0;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(DATA[i], INPUT);
  }

  for (int i = 0; i < 7; i++) {
    pinMode(ADDR[i], OUTPUT);
  }

  Serial.begin(115200);

  pinMode(WE, OUTPUT);
  pinMode(OE, OUTPUT);

  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);

  Serial.println("EEPROM ready. Type a string to write starting at 0x00.");
}

void loop() {
  static String inputBuffer = "";

  // Read characters from serial
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        if (inputBuffer == "Read") {
          readEEPROM();  // Call the function to read and print non-cleared addresses
        } else if (inputBuffer == "Clear") {
          clearEERPOM();
        } else {
          storeStringToEEPROM(inputBuffer);  // Store string if it's not a "Read" command
          Serial.println("Wrote to EEPROM:");
          printEEPROMString();
        }
        inputBuffer = "";  // Clear buffer for next input
      }
    } else {
      inputBuffer += c;
    }
  }
}

void storeStringToEEPROM(const String& str) {
  // Clear first
  for (unsigned int addr = 0; addr < 128; addr++) {
    writeByteWithPolling(addr, 0xFF);
  }

  // Write characters with null terminator
  for (unsigned int i = 0; i < str.length(); i++) {
    writeByteWithPolling(i, str[i]);
  }
  writeByteWithPolling(str.length(), '\0');  // Null terminator
}

void printEEPROMString() {
  Serial.print("Read from EEPROM: ");
  for (unsigned int addr = 0; addr < 128; addr++) {
    char c = (char)readByte(addr);
    if (c == '\0' || c == 0xFF) break;
    Serial.print(c);
  }
  Serial.println();
}

void readEEPROM() {
  Serial.println("Reading EEPROM...");

  bool foundNonCleared = false;
  for (unsigned int addr = 0; addr < 128; addr++) {
    byte val = readByte(addr);
    if (val != 0xFF) {
      Serial.print("Non-cleared address 0x");
      Serial.print(addr, HEX);
      Serial.print(": ");
      Serial.println(val, HEX);
      foundNonCleared = true;
    }
  }

  if (!foundNonCleared) {
    Serial.println("No non-cleared data found in EEPROM.");
  }
}

void checkEEPROMData() {
  for (int addr = 0; addr < 128; addr++) {
    byte val = readByte(addr);
    Serial.print("Value at address ");
    Serial.print(addr, HEX);
    Serial.print(": ");
    Serial.println(val, HEX);
  }
}

void setAddress(unsigned char addr) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(ADDR[i], (addr >> i) & 1);
  }
}

void writeByte(unsigned char addr, unsigned char data) {
  setAddress(addr);

  for (int i = 0; i < 8; i++) {
    pinMode(DATA[i], OUTPUT);
    digitalWrite(DATA[i], (data >> i) & 1);
  }

  digitalWrite(WE, LOW);
  delayMicroseconds(1);
  digitalWrite(WE, HIGH);
  delayMicroseconds(200);

  for (int i = 0; i < 8; i++) {
    pinMode(DATA[i], INPUT);
  }
}

void writeByteWithPolling(unsigned char addr, unsigned char data) {
  writeByte(addr, data);

  while (readByte(addr) != data);
}

byte readByte(unsigned char addr) {
  setAddress(addr);
  digitalWrite(OE, LOW);
  delayMicroseconds(1);

  byte data = 0;
  for (int i = 0; i < 8; i++) {
    data |= (digitalRead(DATA[i]) << i);
  }

  digitalWrite(OE, HIGH);
  return data;
}

void clearEERPOM() {
  for (unsigned int addr = 0; addr < 128; addr++) {
    Serial.print("Clearing address 0x");
    Serial.println(addr, HEX);
    writeByte(addr, 0xFF);
    delay(20);
  }
}
