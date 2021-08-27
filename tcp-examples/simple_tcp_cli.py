# -*- coding: utf-8 -*-
import socket
import time


if __name__ == "__main__":
    HOST = "192.168.1.115"
    PORT = 18989

    cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while 1:
        try:
            cli_socket.connect((HOST, PORT))
            print("Connected to {}:{}".format(HOST, PORT))
            break
        except ConnectionRefusedError as e:
            print("Server refused connection, retrying ...")
            time.sleep(1)
            continue
    
    while 1:
        data = cli_socket.recv(512)
        if len(data) == 0:
            break
        print("Recv:", data.decode("utf-8"))

    cli_socket.close()
