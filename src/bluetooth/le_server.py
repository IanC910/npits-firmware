
from bluetooth import btfpy

def _callback(clientnode, operation, cticn):
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

def run_le_server(write_queue, read_req_queue, read_resp_queue):
    print("Starting LE server")

    def server_callback(clientnode, operation, cticn):
        cb_result = _callback(clientnode, operation, cticn)

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

    LE_SERVER_CALLBACK_PERIOD_ds = 5
    btfpy.Le_server(server_callback, LE_SERVER_CALLBACK_PERIOD_ds)
    print("LE server stopped")

    btfpy.Close_all()