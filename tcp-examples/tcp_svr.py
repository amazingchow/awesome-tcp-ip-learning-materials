# -*- coding: utf-8 -*-
import socket
from time import gmtime, strftime


if __name__ == "__main__":
    HOST = "0.0.0.0"
    PORT = 18989

    svr_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    svr_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    svr_socket.bind((HOST, PORT))
    svr_socket.listen(5)

    srv_cnt = 0
    while 1:
        if srv_cnt == 100:
            break
        print("Waiting for client connection ...")
        conn, addr = svr_socket.accept()
        conn.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
        print("Connected by", addr)
        srv_cnt += 1
        with conn:
            while 1:
                try:
                    data = conn.recv(512)
                except ConnectionResetError as e:
                    print("Client connection closed")
                    break
                if len(data) == 0:
                    conn.shutdown(socket.SHUT_RDWR)
                    while conn.recv(512):
                        pass
                    conn.close()
                    break
                else:
                    msg = data.decode("utf-8", errors="ignore").strip()
                    if msg == "exit" or msg == "quit":
                        conn.shutdown(socket.SHUT_RDWR)
                        conn.close()
                        break
                    else:
                        print("Recv:", data.decode("utf-8", errors="ignore"))
                        conn.sendall("{}".format(strftime("%Y-%m-%d %H:%M:%S", gmtime())).encode("utf-8"))

    svr_socket().close()
