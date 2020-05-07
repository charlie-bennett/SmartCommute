import bluetooth
import sys
import os
import threading
import time




def send_bluetooth_to_device(my_thread):
    uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"  # arbitrary set
    service_matches = bluetooth.find_service(uuid=uuid, address=addr)

    if len(service_matches) == 0:
        print("Couldn't find the SampleServer service.")
        sys.exit(0)

    first_match = service_matches[0]
    port = first_match["port"]
    name = first_match["name"]
    host = first_match["host"]

    print("Connecting to \"{}\" on {}".format(name, host))

    # Create the client socket
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((host, port))

    print("Connected. Type something...")
    while True:
        data = my_thread.read_data_out()
        if not data:
            break
        sock.send(data)
        my_thread.yield()

    sock.close()

def recv_bluetooth_from_device(my_thread):
    # from source code exaples
    server_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    server_sock.bind(("", bluetooth.PORT_ANY))
    server_sock.listen(1)

    port = server_sock.getsockname()[1]

    uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

    bluetooth.advertise_service(server_sock, "SampleServer", service_id=uuid,
                                service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                                profiles=[bluetooth.SERIAL_PORT_PROFILE],
                                # protocols=[bluetooth.OBEX_UUID]
                                )

    print("Waiting for connection on RFCOMM channel", port)

    client_sock, client_info = server_sock.accept()
    print("Accepted connection from", client_info)

    try:
        while True:
            data = client_sock.recv(1024)
            my_thread.write_data_in(data)
            my_thread.yeild()
            if not data:
                break
            print("Received", data)
    except OSError:
        pass

    print("Disconnected.")

    client_sock.close()
    server_sock.close()


class ThreadInstance:
    threads = []
    data_in = []
    data_out = []
    in_lock = threading.Lock()
    out_lock = threading.Lock()
    ok2send = 1

    def run_all_threads(cls):
        for thread in ThreadInstance.threads:
            thread.start()

    def join_all_threads(cls):
        for thread in ThreadInstance.threads:
            thread.join()
    def __init__(self, thread_name, function, args):
        threading.Thread__init__(self)
        self.thread_name = thread_name
        self.function = function
        ThreadInstance.threads.append(self)
    def yeild():
        time.sleep(0)
    def run(self):
        print("Starting {}".format(self.thread_name))
        self.function(self)
        print("Ending {}".format(self.thread_name))
    def write_data_in(self, data):
        while 1:
            if ThreadInstance.in_lock.acquire(timeout=1):
                if (data == "Z"):
                    ThreadInstance.toggle_output()
                ThreadInstance.data_in.append(data)
                break
    def write_data_out(self, data):
        while 1:
            if ThreadInstance.out_lock.acquire(timeout=1):
                ThreadInstance.data_out.append(data)
                break

    def read_data_in(self):
        while 1:
            if ThreadInstance.in_lock.acquire(timeout=1):
                return ThreadInstance.data_in.pop(0)
        return -1
    def read_data_out(self):
        while 1:
            if ThreadInstance.out_lock.acquire(timeout=1):
                if (ThreadInstance.ok2send):
                    time.sleep(0)
                    ThreadInstance.out_lock.acquire(True) # wait for re-enable

                return ThreadInstance.data_out.pop(0)
        return -1
    def toggle_output(cls):
        if (ThreadInstance.ok2send):
            ThreadInstance.ok2send  = 0
            data_out.acquire(True)

        else:
            data_out.release()
            ThreadInstance.ok2send = 1
