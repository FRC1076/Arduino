#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <ArduinoJson.h>

#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define ROBOT_IP  IPAddress(10, 10, 76, 2)
#define SONAR_PORT 8888

#define NUMPIXELS 64
#define PIN 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

boolean debugging = true;

const int microsperinch = 147;
int interruptPin[2] =  { 2, 3 }; 
unsigned long startPulse[2] = { 0, 0 };
unsigned long endPulse[2] = { 0, 0 };
boolean gotPulse[2] = { false, false };
unsigned long pulseWidth[2] = { 0, 0 };
int localPort = 8888;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress robotip(10, 10, 76, 245);
IPAddress myip(10, 10, 76, 7);

void pulseChange0() {
  pulseChange(0); 
 }
 
void pulseChange1() {
  pulseChange(1); 
 }
void pulseChange(int which) {
   if (digitalRead(interruptPin[which]) == HIGH){
      startPulse[which] = micros();
      return;
   }
   if (digitalRead(interruptPin[which]) == LOW){
      endPulse[which] = micros();
      pulseWidth[which] = (endPulse[which] - startPulse[which]) / microsperinch;
      gotPulse[which] = true;
   }
}
EthernetUDP Udp;
char ReplyBuffer[255]; // a string to send back

void setup() {
  Ethernet.begin(mac, myip);
  Udp.begin(localPort);
  // declare the ledPin as an OUTPUT:

  if (debugging) {
      Serial.begin(9600);
  }
  pinMode(interruptPin[0], INPUT_PULLUP);
  pinMode(interruptPin[1], INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin[0]), pulseChange0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPin[1]), pulseChange1, CHANGE);

   // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
  setAllLEDs(0,0,0);
}


void loop() {
  if (gotPulse[0] || gotPulse[1]) {
    if (debugging) {
     Serial.println("Pulse:");
     Serial.println(pulseWidth[0]);
     setAllLEDs(trunc(200/pulseWidth[0]),trunc(40-(200/pulseWidth[0])),0);
     Serial.println(startPulse[0]);
    }
    gotPulse[0] = false;
    gotPulse[1] = false;
//    Udp.beginPacket(ROBOT_IP, SONAR_PORT);
//    sprintf(ReplyBuffer, "{\"sender\": \"sonar\", \"message\": \"ranges\", \"left front\":%d, \"right front\":%d}", pulseWidth[0], pulseWidth[1]);
//    Udp.write(ReplyBuffer);
//    Udp.endPacket();
    if (debugging) {
      Serial.println("Sent");
    }
    delay(50);
  }
}

void setAllLEDs(int r, int g, int b)
{
  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r,g,b)); 
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
}
