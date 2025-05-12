import RPi.GPIO as GPIO
import time

DATA = [14, 15, 18, 23, 24, 25, 8, 7]
ADDR = [2, 3, 4, 17, 27, 22, 10, 9, 11, 5, 6, 13, 19, 26, 16]
WE = 20
OE = 21

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

for pin in ADDR:
    GPIO.setup(pin, GPIO.OUT)

for pin in DATA:
    GPIO.setup(pin, GPIO.IN)

GPIO.setup(WE, GPIO.OUT)
GPIO.setup(OE, GPIO.OUT)
GPIO.output(WE, GPIO.HIGH)
GPIO.output(OE, GPIO.HIGH)

def set_address(addr):
    for i in range(7):
        GPIO.output(ADDR[i], (addr >> i) & i)

def set_data_mode(mode):
    for pin in DATA:
        GPIO.setup(pin, GPIO.OUT if mode == 'write' else GPIO.IN)

def write_byte(addr, data):
    set_address(addr)
    set_data_mode('write')
    for i in range(8):
        GPIO.output(DATA[i], (data >> i) & 1)
    GPIO.output(WE, GPIO.LOW)
    time.sleep(0.000001)
    GPIO.output(WE, GPIO.HIGH)
    time.sleep(0.0002)
    set_data_mode('read')

def read_byte(addr):
    set_address(addr)
    set_data_mode('write')
    GPIO.output(OE, GPIO.LOW)
    time.sleep(0.000001)
    data = 0
    for i in range(8):
        if GPIO.input(DATA[i]):
            data |= (1 << i)
    
    GPIO.output(OE, GPIO.HIGH)
    return data

def write_byte_with_polling(addr, data):
    write_byte(addr, data)
    while read_byte(addr) != data:
        pass

def clear_eeprom():
    for addr in range(0x8000):
        print(f"Clearing address 0x{addr:02X}")
        write_byte_with_polling(addr, 0xFF)
        # time.sleep(0.02)

def read_eeprom():
    found_non_cleared = False
    for addr in range(0x8000):
        print(f"Reading data at address 0x{addr:02X}")
        byte = read_byte(addr)
        if byte != 0xFF:
            print(f"Non-cleared address at 0x{addr:02X} found value: {byte:02X}")
            found_non_cleared = True
    
    if not found_non_cleared:
        print("EEPROM is fully cleared")

def handle_command(command):
    """Handle the user input command."""
    if command.lower() == "clear":
        print("Clearing EEPROM...")
        clear_eeprom()
    elif command.lower().startswith("write"):
        try:
            parts = command.split()
            addr = int(parts[1], 16)
            data = int(parts[2], 16)
            print(f"Writing 0x{data:02X} to address 0x{addr:02X}")
            write_byte_with_polling(addr, data)
        except IndexError:
            print("Usage: write <address> <data>")
    elif command.lower().startswith("read"):
        try:
            parts = command.split()
            addr = int(parts[1], 16)
            byte = read_byte(addr)
            print(f"Read 0x{byte:02X} from address 0x{addr:02X}")
        except IndexError:
            print("Usage: read <address>")
    elif command.lower() == "exit":
        print("Exiting...")
        return False
    else:
        print("Unknown command!")
    return True

def main():
    """Simple BIOS-like interface."""
    try:
        while True:
            command = input("Enter command (clear/write/read <address> <data>/exit): ")
            if not handle_command(command):
                break
    except KeyboardInterrupt:
        pass
    finally:
        GPIO.cleanup()

if __name__ == "__main__":
    # write_byte_with_polling(0x00, 0x00)
    # clear_eeprom()
    main()