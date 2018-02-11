import json
import socket

UDP_IP = "10.10.76.17"
UDP_PORT = 8888
MESSAGE = {'message': 'light-show', 'show-index': 6}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
message = json.dumps(MESSAGE).encode("utf8")
print(message)
sock.sendto(message, (UDP_IP, UDP_PORT))
