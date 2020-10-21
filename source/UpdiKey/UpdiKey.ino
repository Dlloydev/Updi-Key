/*
   updiKey.ino
   Created: 10-21-2020
   Author: DWL
*/

#include "updif.h"
byte loopcount = 0;
byte signalStatus = 1;

void setup() {
  updiKeyInit();
  pulseHV();
  noInterrupts();
  updiEnable();
  interrupts();
}

void loop() {
  signalStatus = checkSignal();
  if (signalStatus == 0) {
    // re-updify button was just released
    powerCycle();
    pulseHV();
    noInterrupts();
    updiEnable();
    interrupts();
  }
  else if (signalStatus == 1) {
    // signal is now stable high
  }
  else if (signalStatus == 2) {
    // ready to power cycle
    powerCycle();
  }
  else {
    // signal is active (busy)
  }
  delayMicroseconds(20);
}
