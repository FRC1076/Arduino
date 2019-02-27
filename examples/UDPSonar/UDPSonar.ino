/*
 UDPSendReceiveString:
 This sketch receives UDP message strings, prints them to the serial port
 and sends an "acknowledge" string back to the sender

 A Processing sketch is included at the end of file that can be used to send
 and received messages for testing with a computer.

 created 21 Aug 2010
 by Michael Margolis

 This code is in the public domain.
 */


#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0x10, 0x10, 0x76, 0x11
};
IPAddress ip(10, 10, 76, 11);
IPAddress robot_ip(10, 10, 76, 2);

unsigned int remotePort = 5811; //  port for receiver
unsigned int localPort = 5811;  //  same

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);

  Serial.begin(9600);
}

void loop() {  
    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(robot_ip, remotePort);
    int range = sonar.ping_cm();
    Serial.print("Ping returns ");
    Serial.print(range);
    Serial.println(" cm");
    String rangemessage =
      "{\"sender\":\"sonar\",\"range-cm\":"+
      String(sonar.ping_cm())+"}";
    Udp.write(rangemessage.c_str());
    Udp.endPacket();

    // send updates 40 times per second to avoid buffering at receiver
    delay(25);
}

/*
# Python script to monitor the sonar data
# This should be run in the 2019-Competition directory
# so it can get to subsystems by name.
#
# simple utility to connect to sonar sensor at .11
# and log the range-cm it returns.
#
from subsystems.sonarSensor import SonarSensor
import time
import logging
logger = logging.getLogger('monitor_sonar')

sensor = SonarSensor('10.10.76.11', 5811, logger)

while(1):
    sensor.receiveRangeUpdates()
    print("Received: ", sensor.pidGet())
    # get 50 updates per second to stay ahead of the sender
    time.sleep(0.02)

*/
