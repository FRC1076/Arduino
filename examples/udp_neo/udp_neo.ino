


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 512
#define UDP_PACKET_MAX_SIZE 256

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0x10, 0x76, 0x1, 0xB1
};
IPAddress ip(10, 10, 76, 7);

unsigned int localPort = 8777;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "Posted!";          // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
int showType = 0;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  // if there's data available, read a packet
  
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i = 0; i < 4; i++) {
      Serial.print(remote[i], DEC);
      if (i < 3) {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_PACKET_MAX_SIZE);
    packetBuffer[packetSize] = '\0';    // mark the end of the buffer
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    String stringBuffer(packetBuffer);
    //showType = stringBuffer.toInt();

    DynamicJsonBuffer jsonBuffer;
    String input = stringBuffer;
    JsonObject& root = jsonBuffer.parseObject(input);

    // You can use a String to get an element of a JsonObject
    // No duplication is done.
    String message = root[String("message")];
    int num_pixels = root[String("num_pixels")];
    int clear = root[String("clear")];
    //int *pixel_values = root["pixel_values"];
    //Serial.println(message);

    if (clear) {
        strip.clear();
    }

    for (int i=0; i<num_pixels; i++) {
        strip.setPixelColor(root["pixel_values"][i], strip.Color(0,5,5));
    }
    strip.show();

    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }

  // do not be so greedy, wait a bit for another attempt
  delay(10);
}