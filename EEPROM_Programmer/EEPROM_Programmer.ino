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

int read_byte(int addr) {
  set_address(addr);
  set_data_mode("read");
  digitalWrite("OE", LOW);
  delayMicroseconds(1);
  int data = 0;
  for (int i = 0; i < 7; i++) {
    if (digitalRead(DATA[i]))
      data |= (1 << i);
  }

  digitalWrite(OE, HIGH);
  return data;
}

void write_byte_with_polling(int addr, int data) {
  int current = read_byte(addr);

  if (current == data)
    return;

  if (current != 0xFF)
  {
    write_byte(addr, 0xFF);
    delay(10);
    if (read_byte(addr) != 0xFF)
    {
      Serial.print("Warning: failed to erase 0x");
      Serial.println(addr);
      return;
    }
  }

  write_byte(addr, data);

  for (int i = 0; i < 100; i++)
  {
    if (read_byte(addr) == data)
      return;
    delay(5);
  }

  Serial.print("Warning: Failed to write 0x");
  Serial.print(data);
  Serial.print(" to 0x");
  Serial.println(addr);
}

void clear_eeprom()
{
  for (int i = 0; i < 0x8000; i++)
  {
    Serial.print("Clearing EEPROM at address 0x");
    Serial.println(addr);
    write_byte_with_polling(i, 0xFF);
  }
}

void read_eeprom()
{
  bool non_cleared = false;
  for (int i = 0; i < 0x8000; i++)
  {
    Serial.print("Reading data at address 0x");
    Serial.println(i);

    int byte = read_byte(i);

    if (byte != 0xFF)
    {
      Serial.print("Non-cleared address at 0x");
      Serial.print(i);
      Serial.print(" found value: 0x");
      Serial.println(byte);
      non_cleared = true;
    }

    delay(0.5);
  }

  if (non_cleared == false)
    Serial.println("EEPROM is fully cleared");   
}

bool handle_command(String command)
{
  command.trim();
  command.toLowerCase();

  if (command == "clear")
  {
    Serial.println("Clearing EEPROM...");
    clear_eeprom();
  }
  else if (command.startsWith("write"))
  {
    int firstSpace = command.indexOf(' ');
    int secondSpace = command.indexOf(' ', firstSpace + 1);

    if (firstSpace == -1 || secondSpace == -1)
    {
      Serial.println("Usage: write <address> <data>");
      return true;
    }

    String addrString = command.substring(firstSpace + 1, secondSpace);
    String dataString = command.substring(secondSpace + 1);
    
    int addr = (int) strtol(addrString.c_str(), NULL, 16);
    int data = (int) strtol(dataString.c_str(), NULL, 16);

    if (addr >= 0 && addr < 0x8000)
    {
      Serial.print("Writing 0x");
      Serial.print(data, HEX);
      Serial.print(" to address 0x");
      Serial.println(addr, HEX);
      write_byte_with_polling(addr, data);
    }
    else
    {
      Serial.println("Address out of range!");
    }
  }
}
