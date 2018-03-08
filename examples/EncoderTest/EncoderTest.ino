/*
  Test CIMCoder channel.

  Test a single channel of the CIMCoder attached to pin 2.
  Measure the time at the first and the PULSE_COUNTth pulse.

  Report results in time per 1000 pulses.  (50 revolutions @ 20 pulses / rev)

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/
#include <Arduino1076.h>

#define PULSE_COUNT    999
int interruptPin = 2; 
uint32_t volatile startPulse = 0;
uint32_t volatile endPulse = 0;
uint32_t volatile pulseCount = 0;

void pulseChange() {
   if (digitalRead(interruptPin) == HIGH){
      Serial.print("Click: ");
      Serial.println(pulseCount);
      if (pulseCount == 0) {
        startPulse = millis();
      }
      if (pulseCount == PULSE_COUNT) {
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
  if (pulseCount > PULSE_COUNT) {
    noInterrupts();
    uint32_t pw = endPulse - startPulse;
    uint32_t pc = pulseCount;
    pulseCount = 0;
    interrupts();
    Serial.print("Pulses: ");
    Serial.print(pc);
    Serial.print(" in time: ");
    Serial.print(pw);
    Serial.println(" ms.");
  }
}
