from udp_channel import UDPChannel
import json
import time

brightness = 1.0
update = True
progress = 31

channel = UDPChannel("10.10.76.22", 8899, "10.10.76.8", 8777, 2)

while(update):

    pixels = [hex(x) for x in range(16)] + [progress]
    colors = [hex(x) for x in range(8)] + [x for x in range(8)] + [7]

    data = {
    'sender' : 'reid',
    'message' : 'raw_display',
    'num_pixels' : len(pixels),
    'brightness' : brightness,
    'pixel_values' : pixels,
    'pixel_colors' : colors
    }

    channel.send_to(json.dumps(data))

    progress += 1

    if brightness >= 0.05:
        brightness -= 0.05
        print(brightness)
    else:
        update = False

    time.sleep(0.5)