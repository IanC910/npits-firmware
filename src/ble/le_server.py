import btfpy
import asyncio
from gpiozero import CPUTemperature
import multiprocessing

def callback(clientnode, operation, cticn):
    if operation == btfpy.LE_CONNECT:
        pass
    elif operation == btfpy.LE_READ:
        pass
    elif operation == btfpy.LE_WRITE:
        pass
    elif operation == btfpy.LE_DISCONNECT:
        pass
    elif operation == btfpy.LE_TIMER:
        pass
    elif operation == btfpy.LE_KEYPRESS:
        pass

    return btfpy.SERVER_CONTINUE

def init_bluetooth():
    if btfpy.Init_blue("devices.txt") == 0:
        exit(0)

    print("The local device must be the first entry in devices.txt")
    print("(My Pi) that defines the LE characteristics")
    print("Connection/pairing problems? See notes in le_server.py")

    btfpy.Write_ctic(btfpy.Localnode(), 0, "Hello world PI", 0)

    randadd = [0xD3, 0x56, 0xDB, 0x24, 0x32, 0xA0]
    btfpy.Set_le_random_address(randadd)
    btfpy.Set_le_wait(5000)
    btfpy.Le_pair(btfpy.Localnode(), btfpy.JUST_WORKS, 0)

def run_le_server(write_queue, read_req_queue, read_resp_queue):
    init_bluetooth()
    print("Starting LE server")

    def server_callback(clientnode, operation, cticn):
        cb_result = callback(clientnode, operation, cticn)

        if(not read_req_queue.empty()):
            char_index = read_req_queue.get()
            print("reading index " + str(char_index))
            data = btfpy.Read_ctic(btfpy.Localnode(), char_index)
            read_resp_queue.put((char_index, data))

        elif(not write_queue.empty()):
            char_index, data = write_queue.get()
            print("writing " + str(data))
            btfpy.Write_ctic(btfpy.Localnode(), char_index, data, 0)

        return cb_result

    LE_SERVER_CALLBACK_PERIOD_DS = 5
    btfpy.Le_server(server_callback, LE_SERVER_CALLBACK_PERIOD_DS)
    print("LE server stopped")