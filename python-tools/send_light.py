import json
import socket
import sys

shows = ['off', 'red', 'green', 'blue', 'chaseWhite', 'chaseRed', 'chaseBlue',
        'rainbow', 'rainbowCycle', 'theaterChaseRainbow']
showName = sys.argv[1]
if showName in shows:
    showIndex = shows.index(showName)
else:
    print("I do not understand your show name. These are your options: ")
    print(shows)
    sys.exit()


UDP_IP = "10.10.76.17"
UDP_PORT = 8888
MESSAGE = {'message': 'light-show', 'show-index': showIndex}

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
message = json.dumps(MESSAGE).encode("utf8")
print(message)
sock.sendto(message, (UDP_IP, UDP_PORT))
