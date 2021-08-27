# -*- coding: utf-8 -*-
import json
import requests
import time


if __name__ == "__main__":
    send_cnt = 0
    while 1:
        if send_cnt == 300:
            break
        r = requests.get("http://192.168.1.115:18988/books")
        if r.status_code != 200:
            print("Failed to send request <status code:", r.status_code, ">")
            break
        print("\n{}\n".format(json.dumps(r.json(), indent=4)))
        time.sleep(2)
        send_cnt += 1
