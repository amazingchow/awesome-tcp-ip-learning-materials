# -*- coding: utf-8 -*-
import faker
__Faker = faker.Faker()
import socket
import time


if __name__ == "__main__":
    HOST = "192.168.1.115"
    PORT = 18989

    cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cli_socket.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
    
    while 1:
        try:
            cli_socket.connect((HOST, PORT))
            print("Connected to {}:{}".format(HOST, PORT))
            break
        except ConnectionRefusedError as e:
            print("Server refused connection, retrying ...")
            time.sleep(1)
            continue
    
    send_cnt = 0
    while 1:
        if send_cnt == 300:
            break
        try:
            msg = "Hello, my name is {}, what time is it now?".format(__Faker.name())
            cli_socket.sendall(msg.encode("utf-8"))
        except ConnectionResetError as e:
            print("Server connection closed")
            break
        data = cli_socket.recv(512)
        print("Recv:", data.decode("utf-8"))
        time.sleep(2)
        send_cnt += 1

    cli_socket.shutdown(socket.SHUT_RDWR)
    cli_socket.close()
