import socket

send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
send_sock.bind(('10.10.76.23', 8888))
message = '{ "sender" : "spill", "clear" : 1, "num_pixels" : 5, "pixel_values" : [ 1, 13, 19, 27, 35 ] }'
send_sock.sendto(message.encode(), ('10.10.76.7', 8777))

