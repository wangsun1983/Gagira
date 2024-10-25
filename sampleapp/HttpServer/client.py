import socket
import threading
import time
import http.client

import sys

count = 0

url = "127.0.0.1:1234"

client = http.client.HTTPConnection(url)
client.request("GET","/simpleget/100")
r1 = client.getresponse()
print(r1.read().decode("utf-8"))
# client = http.client.HTTPConnection(url)
# client.request("GET","/simpleget2/200")
# r1 = client.getresponse()

client.close();

print("finish!!!")