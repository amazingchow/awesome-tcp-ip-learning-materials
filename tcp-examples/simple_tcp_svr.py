# -*- coding: utf-8 -*-
import socket
from time import gmtime, strftime


if __name__ == "__main__":
    HOST = "0.0.0.0"
    PORT = 18989

    svr_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    svr_socket.bind((HOST, PORT))
    svr_socket.listen(5)

    srv_cnt = 0
    while 1:
        if srv_cnt == 100:
            break
        print("Waiting for client connection ...")
        conn, addr = svr_socket.accept()
        print("Connected by", addr)
        srv_cnt += 1
        conn.shutdown(socket.SHUT_RDWR)
        conn.close()

    svr_socket().close()
