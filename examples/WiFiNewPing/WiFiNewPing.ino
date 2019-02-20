#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPI.h> 
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <WiFiUdp.h>
#include <NewPing.h>
#include <Arduino1076.h>

/**************************************************************************************
 * this example shows how to set a static IP configuration for the ESP
 * although the IP shows in the config portal, the changes will revert 
 * to the IP set in the source file.
 * if you want the ability to configure and persist the new IP configuration
 * look at the FS examples, which save the config to file
 *************************************************************************************/


//default custom static IP
//char static_ip[16] = "10.0.1.59";
//char static_gw[16] = "10.0.1.1";
//char static_sn[16] = "255.255.255.0";
const unsigned int TRIGGER_PIN = 12;   // Arduino pin tied to trigger pin on the ultrasonic sensor.
const unsigned int ECHO_PIN = 11;      // Arduino pin tied to echo pin on the ultrasonic sensor.
const unsigned int MAX_DISTANCE = 200; // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
bool doing_sonar = false;
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[255]; // a string to send back
unsigned int localPort = 5812;

WiFiUDP Udp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set static ip
  //block1 should be used for ESP8266 core 2.1.0 or newer, otherwise use block2

  //start-block1
  //IPAddress _ip,_gw,_sn;
  //_ip.fromString(static_ip);
  //_gw.fromString(static_gw);
  //_sn.fromString(static_sn);
  //end-block1

  //start-block2
  IPAddress _ip = IPAddress(10, 10, 76, 12);
  IPAddress _gw = IPAddress(10, 10, 76, 1);
  IPAddress _sn = IPAddress(255, 255, 255, 0);
  //end-block2
  
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("Arduino1076", "1076")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Udp.begin(localPort);

  while (!doing_sonar) {
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
        Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");
        Serial.println(packetBuffer);

        doing_sonar = true;
      }
      delay(100);
   }

}

void loop() {
    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    sprintf(ReplyBuffer, "{ \"range-cm\" : %d }", sonar.ping_cm());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
    delay(100);  
}
