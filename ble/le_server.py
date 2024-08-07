import btfpy
import asyncio
import random
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

    btfpy.Write_ctic(btfpy.Localnode(), 2, "Hello world PI", 0)

    randadd = [0xD3, 0x56, 0xDB, 0x24, 0x32, 0xA0]
    btfpy.Set_le_random_address(randadd)
    btfpy.Set_le_wait(5000)
    btfpy.Le_pair(btfpy.Localnode(), btfpy.JUST_WORKS, 0)

def run_le_server(update_queue):
    init_bluetooth()
    print("Starting LE server")

    def server_callback(clientnode, operation, cticn):
        result = callback(clientnode, operation, cticn)
        if not update_queue.empty():
            temp_update = update_queue.get()
            btfpy.Write_ctic(btfpy.Localnode(), 2, temp_update, 0)
        return result

    btfpy.Le_server(server_callback, 0)
    print("LE server stopped")

async def update_ctic(update_queue):
    print("Ctic update started")
    while True:
        out = "Random Value: " + str(random.randint(1, 10000))
        print(out)
        update_queue.put(out)
        await asyncio.sleep(5)

async def main():
    update_queue = multiprocessing.Queue()

    # Start the LE server in a separate process
    le_server_process = multiprocessing.Process(target=run_le_server, args=(update_queue,))
    le_server_process.start()

    try:
        # Start the temperature update coroutine
        await update_ctic(update_queue)
    finally:
        # Ensure the LE server process is terminated
        le_server_process.terminate()
        le_server_process.join()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    finally:
        btfpy.Close_all()
