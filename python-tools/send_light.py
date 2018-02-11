import json
import socket
import sys

UDP_IP = "10.10.76.17"
UDP_PORT = 8888
MESSAGE = {'message': 'light-show', 'show-index': sys.argv[1]}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
message = json.dumps(MESSAGE).encode("utf8")
print(message)
sock.sendto(message, (UDP_IP, UDP_PORT))
