import serial
import serial.tools.list_ports
import threading

def list_serial_ports():
    ports = list(serial.tools.list_ports.comports())
    print("Available Serial Ports: ")
    for i, port in enumerate(ports):
        print(f"{i} : {port.device} - {port.description}")
    return ports

def choose_port(ports):
    index = int(input("Select port index: "))
    return ports[index].device

def read_serial(ser: serial.Serial):
    while True:
        if ser.in_waiting:
            data = ser.readline().decode('utf-8', errors='ignore').strip()
            print(f"[RECV] {data}")

def write_serial(ser: serial.Serial):
    while True:
        msg = input("Enter a message to send: ")
        print("\n")
        ser.write((msg + '\n').encode())

def main():
    ports = list_serial_ports()
    if not ports:
        print("No Serial Ports found")
        return
    
    port = choose_port(ports)
    baud = int(input("Enter buad rate: "))

    ser = serial.Serial(port, baud, timeout=1)
    print(f"Connnected to {port} at {baud} baudrate")

    threading.Thread(target=read_serial, args=(ser,), daemon=True).start()
    write_serial(ser)

if __name__ == "__main__":
    main()
