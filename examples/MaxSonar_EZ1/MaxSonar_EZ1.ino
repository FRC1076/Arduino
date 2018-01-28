/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <ArduinoJson.h>
#include <Arduino1076.h>
#include <EthernetUdp.h>  // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <NewPing.h>

const int microsperinch = 147;
int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int interruptPin = 2; 
int startPulse = 0;
int endPulse = 0;
boolean gotPulse = false;
int pulseWidth = 0;
int localPort = 8888;
DynamicJsonBuffer jsonBuffer;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress robotip(10, 10, 76, 245);
IPAddress myip(10, 10, 76, 7);

void pulseChange() {
   if (digitalRead(interruptPin) == HIGH){
      startPulse = micros();
      return;
   }
   if (digitalRead(interruptPin) == LOW){
      endPulse = micros();
      pulseWidth = (endPulse - startPulse) / microsperinch;
      gotPulse = true;
   }
}
EthernetUDP Udp;
char ReplyBuffer[255]; // a string to send back

void setup() {
  Ethernet.begin(mac, myip);
  Udp.begin(localPort);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pulseChange, CHANGE);
  String input("{ \"sender\" : \"sonar\" }");
  JsonObject& root = jsonBuffer.parseObject(input);
  if (root[String("sender")] == String("sonar")) {
    Serial.println("json passed");
}

}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  // turn the ledPin on
  digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  // turn the ledPin off:
  digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  if (gotPulse) {
    Serial.println("Pulse:");
    Serial.println(pulseWidth);
    gotPulse = false;
  }
  delay(100);
  Serial.println(sensorValue);

  Udp.beginPacket(ROBOT_IP, SONAR_PORT);
  sprintf(ReplyBuffer, "{\"sender\": \"sonar\", \"message\": \"ranges\", \"left front\":%d, \"right front\":%d}", pulseWidth, pulseWidth);
  Udp.write(ReplyBuffer);
  Udp.endPacket();
  Serial.println("Sent");
  delay(500);
}
