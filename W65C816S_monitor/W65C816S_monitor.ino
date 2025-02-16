const char ADDR[] = {33, 31, 29, 27, 25, 23, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40};
const char DATA[] = {37, 39, 41, 43, 45, 47, 49, 51};
#define CLOCK_OUT 13
#define READ_WRITE 53

void setup() {
  for (int n = 0; n < 16; n += 1) {
    pinMode(ADDR[n], INPUT);
  }
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], INPUT);
  }
  pinMode(READ_WRITE, INPUT);
  pinMode(CLOCK_OUT, OUTPUT);
  
  Serial.begin(57600);
}

void loop() {
  digitalWrite(CLOCK_OUT, HIGH);
  delay(10);

  logCPU();

  digitalWrite(CLOCK_OUT, LOW);
  delay(10);
}

void logCPU() {
  char output[15];

  unsigned int address = 0;
  for (int i = 0; i < 16; i++) {
    int bit = digitalRead(ADDR[i]) ? 1 : 0;
    Serial.print(bit);
    address = (address << 1) + bit;
  }

  Serial.print("  ");

  unsigned int data = 0;
  for (int i = 0; i < 8; i++) {
    int bit = digitalRead(DATA[i]) ? 1 : 0;
    Serial.print(bit);
    data = (data << 1) + bit;
  }

  sprintf(output, "   %04x  %c  %02x", address, digitalRead(READ_WRITE) ? 'r' : 'w', data);
  Serial.println(output);
}
