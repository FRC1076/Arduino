#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <ArduinoJson.h>
#include <Arduino1076.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008

boolean debugging = false;

const double microspercm = 58.3;
int interruptPin[2] =  { 3, 2 }; 
unsigned long startPulse[2] = { 0, 0 };
unsigned long endPulse[2] = { 0, 0 };
boolean gotPulse[2] = { false, false };
unsigned long pulseWidth[2] = { 0, 0 };
unsigned long range[0];
int localPort = SONAR_PORT;
DynamicJsonBuffer jsonBuffer;

byte mac[] = SONAR_MAC;

IPAddress robotip(10, 10, 76, 2);
IPAddress myip(10, 10, 76, 11);

void pulseChange0() {
  pulseChange(0); 
 }
 
void pulseChange(int which) {
    if (!gotPulse[which]) {
      if (digitalRead(interruptPin[which]) == HIGH){
          startPulse[which] = micros();
          return;
      }
      if (digitalRead(interruptPin[which]) == LOW){
          endPulse[which] = micros();
          gotPulse[which] = true;
      }
    }
}
EthernetUDP Udp;
char SendBuffer[255]; // a string to send back

void setup() {
  Ethernet.begin(mac, myip);
  Udp.begin(localPort);
  // declare the ledPin as an OUTPUT:

  if (debugging) {
      Serial.begin(9600);
  }
  pinMode(interruptPin[0], INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(interruptPin[0]), pulseChange0, CHANGE);
  
  if (debugging) {
      String input("{ \"sender\" : \"sonar\" }");
      JsonObject& root = jsonBuffer.parseObject(input);
      if (root[String("sender")] == String("sonar")) {
          Serial.println("json passed");
      }
  }
}

void loop() {
  if (gotPulse[0]) {
    range[0] = round((endPulse[0] - startPulse[0]) / microspercm);
    if (debugging) {
     Serial.print("Range: ");
     Serial.print(range[0]);
     Serial.print(" cm");
     Serial.print("From: ");
     Serial.print(endPulse[0]);
     Serial.print(" - ");
     Serial.print(startPulse[0]);
     Serial.print(" = ");
     Serial.println(endPulse[0]-startPulse[0]);
    }
    Udp.beginPacket(ROBOT_IP, localPort);
    sprintf(SendBuffer, "{\"sender\": \"sonar\", \"message\": \"height\", \"range-cm\":%u}", range[0]);
    Udp.write(SendBuffer);
    Udp.endPacket();
    if (debugging) {
      Serial.println("Sent");
    }
    gotPulse[0] = false;
    delay(100);
  }
}
