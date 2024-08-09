
import btfpy
import multiprocessing

import le_server

class BLE_Interface:
    def __init__(self):
        self._write_queue = multiprocessing.Queue()
        self._read_req_queue = multiprocessing.Queue()
        self._read_resp_queue = multiprocessing.Queue()

        # Start the LE server in a separate process
        self._le_server_process = multiprocessing.Process(
            target = le_server.run_le_server,
            args = (self._write_queue, self._read_req_queue, self._read_resp_queue)
        )
        self._le_server_process.start()

    def __del__(self):
        # Ensure the LE server process is terminated
        self._le_server_process.terminate()
        self._le_server_process.join()
        btfpy.Close_all()

    def write(self, char_index, data):
        self._write_queue.put((char_index, data))

    def request_read(self, char_index):
        self._read_req_queue.put(char_index)

    def receive_read(self):
        if(not self._read_resp_queue.empty()):
            return self._read_resp_queue.get()

        return None