import websocket
import threading

import sys

url = "ws://127.0.0.1:1234/simpleget"
ws = websocket.WebSocket()
ws.connect(url)
msg = "Hello, World"
ws.send(msg)
recvdata = ws.recv()
print(recvdata)
ws.close()

url = "ws://127.0.0.1:1234/complete"
ws = websocket.WebSocket()
ws.connect(url)
msg = "He"
ws.send(msg)
recvdata = ws.recv()
print(recvdata)
ws.close()