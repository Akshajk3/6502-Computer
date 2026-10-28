import RPi.GPIO as GPIO
import time
import sys

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
    for i in range(15):
        GPIO.output(ADDR[i], (addr >> i) & 1)

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
    set_data_mode('read')
    GPIO.output(OE, GPIO.LOW)
    time.sleep(0.000001)
    data = 0
    for i in range(8):
        if GPIO.input(DATA[i]):
            data |= (1 << i)
    
    GPIO.output(OE, GPIO.HIGH)
    return data

def write_byte_with_polling(addr, data):
    current = read_byte(addr)
    
    # Skip if data already correct
    if current == data:
        return

    # Only erase if needed
    if current != 0xFF:
        write_byte(addr, 0xFF)
        time.sleep(0.01)  # Erase delay
        # Confirm erase
        if read_byte(addr) != 0xFF:
            print(f"Warning: Failed to erase 0x{addr:04X}")
            return

    # Write new data
    write_byte(addr, data)
    
    # Poll to confirm write
    for _ in range(100):
        if read_byte(addr) == data:
            return
        time.sleep(0.005)  # Polling delay

    print(f"Warning: Failed to write 0x{data:02X} to 0x{addr:04X}")

def clear_eeprom():
    for addr in range(0x8000):
        print(f"Clearing address 0x{addr:02X}")
        write_byte_with_polling(addr, 0xFF)
        # time.sleep(0.02)

def read_eeprom():
    found_non_cleared = False
    for addr in range(0x8000):
        print(f"Reading data at address 0x{addr:04X}")
        byte = read_byte(addr)
        if byte != 0xFF:
            print(f"Non-cleared address at 0x{addr:04X} found value: {byte:02X}")
            found_non_cleared = True
        time.sleep(0.0005)  # Add short delay to let bus settle
    
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
        parts = command.split()
        if len(parts) == 1:
            print("Reading full EEPROM...")
            read_eeprom()
        else:
            try:
                addr = int(parts[1] , 16)
                byte = read_byte(addr)
                print(f"Read 0x{byte:02X} from address 0x{addr:04X}")
            except ValueError:
                print("Usage: read <address>")
    elif command.lower() == "exit":
        print("Exiting...")
        return False
    else:
        print("Unknown command!")
    return True

def write_rom_to_eeprom(filename, max_retries=3):
    try:
        with open(filename, "rb") as rom:
            contents = rom.read()
            print(f"Writing {len(contents)} bytes from {filename}")
            
            failed_addrs = []

            # First pass
            for addr, byte in enumerate(contents):
                print(f"Writing 0x{byte:02X} to 0x{addr:04X}")
                prev = read_byte(addr)
                write_byte_with_polling(addr, byte)
                if read_byte(addr) != byte:
                    failed_addrs.append((addr, byte))

            # Retry failed writes
            for attempt in range(1, max_retries + 1):
                if not failed_addrs:
                    break
                print(f"\nRetry pass {attempt}: fixing {len(failed_addrs)} failed cells...")
                remaining = []
                for addr, byte in failed_addrs:
                    write_byte_with_polling(addr, byte)
                    if read_byte(addr) != byte:
                        remaining.append((addr, byte))
                failed_addrs = remaining

            if failed_addrs:
                print("\nWARNING: Some addresses still failed after retries:")
                for addr, byte in failed_addrs:
                    actual = read_byte(addr)
                    print(f"  Addr 0x{addr:04X}: expected 0x{byte:02X}, found 0x{actual:02X}")
            else:
                print("\nUpload successful. All bytes verified.")
    except FileNotFoundError:
        print(f"Error: File not found! {filename}")
    except Exception as e:
        print(f"Error: {e}")

def main():
    """Simple BIOS-like interface. or file writer"""
    if len(sys.argv) == 2:
        filename = sys.argv[1]
        write_rom_to_eeprom(filename)
        return

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