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
#include <Arduino1076.h>

int interruptPin = 2; 
uint32_t volatile startPulse = 0;
uint32_t volatile endPulse = 0;
uint32_t volatile pulseCount = 0;

void pulseChange() {
   //Serial.print("Interrupt: ");
   if (digitalRead(interruptPin) == HIGH){
      //Serial.println("HIGH");
      if (pulseCount == 0) {
        startPulse = millis();
      }
      if (pulseCount == 1000) {
        endPulse = millis();
      }
      pulseCount++;
      return;
   }
}

void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), pulseChange, CHANGE);
}

void loop() {
  if (pulseCount > 1000) {
    Serial.print("Pulses: ");
    Serial.print(pulseCount);
    Serial.print("in time: ");
    Serial.print(endPulse - startPulse);
    Serial.println(" ms.");
    pulseCount = 0;
  }
}
