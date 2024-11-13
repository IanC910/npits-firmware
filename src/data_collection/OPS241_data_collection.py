
import serial

SERIAL_PORT = 'COM6'

def send_serial_cmd(command):
    data_for_send_bytes = str.encode(command)
    ser.write(data_for_send_bytes)

# Main Loop
if(__name__ == "__main__"):
    ser = serial.Serial(
        port            = SERIAL_PORT,
        baudrate        = 115200,
        parity          = serial.PARITY_NONE,
        stopbits        = serial.STOPBITS_ONE,
        bytesize        = serial.EIGHTBITS,
        timeout         = 0.5,
        writeTimeout    = 0.5
    )

    ser.flushInput()
    ser.flushOutput()

    send_serial_cmd('??')

    for i in range(10000):
        match(i):
            case 20:
                send_serial_cmd('o9')
            case 30:
                send_serial_cmd('F1')
            case 40:
                send_serial_cmd('oM')
            case 50:
                send_serial_cmd('od')
            case 60:
                send_serial_cmd('oD')
            case _:
                print("", end = "")

        if(i % 7 == 0):
            send_serial_cmd('AX')
        line_bytes = ser.readline()
        Ops241_rx_bytes_length = len(line_bytes)
        line = line_bytes.decode("utf-8")
        print(line, end = "")
