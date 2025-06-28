const char DATA[] = {};
const char ADDR[] = {};

#define WE 11
#define OE 12

void setup() {
  pinMode(WE, OUTPUT);
  pinMode(OE, OUTPUT);

  for (int i = 0; i < 15; i++) {
    pinMode(ADDR[i], OUTPUT);
  }

  for (int i = 0; i < 7; i++) {
    pinMode(DATA[i], OUTPUT);
  }

  Serial.begin(115200);
}

void loop() {

}

void set_address(int addr) {
  for (int i = 0; i < 15; i++) {
    digitalWrite(ADDR[i], (addr >> i) & i);
  }
}

void set_data_mode(String mode) {
  for (int i = 0; i < 7; i++) {
    pinMode(DATA[i], (mode == "write") ? OUTPUT : INPUT);
  }
}

void write_byte(int addr, int data) {
  set_address(addr);
  set_data_mode("write");
  for (int i = 0; i < 7; i++) {
    digitalWrite(DATA[i], (data >> i) & 1);
  }
  digitalWrite(WE, LOW);
  delayMicroseconds(1);
  digitalWrite(WE, HIGH);
  delayMicroseconds(2);
  set_data_mode("read");
}

void read_byte(int addr) {
  set_address(addr);
  set_data_mode("read");
  digitalWrite("OE", LOW);
  delayMicroseconds(1);
  int data = 0;
  for (int i = 0; i < 7; i++) {
    if (digitalRead(DATA[i]))
      data |= (1 << i);
  }
}