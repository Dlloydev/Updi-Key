/*
   updif.h
   Created: 10-21-2020
   Author: DWL
*/

#include <util/delay.h>
#include "updif.h"

bool lastState = true;
bool updifyPressed = false;
uint16_t lowCount = 0;
uint16_t highCount = 0;
uint8_t stableCount = 0;

void updiKeyInit(void) {
  // inputs
  PORTA.DIRCLR = UPDIFY_PUSHBUTTON_BM;
  PORTA.UPDIFY_PUSHBUTTON_CTRL |= PORT_PULLUPEN_bm; // enable pullup
  PORTA.DIRCLR = UPDI_TARGET_BM;
  // outputs
  PORTA.DIRSET = POWER_SWITCH_ENABLE_BM;
  PORTA.OUTSET = POWER_SWITCH_ENABLE_BM; // set power
  PORTA.DIRSET = HV_ENABLE_BM;
  PORTA.DIRSET = UPDI_ENABLE_BM;
}

void bitDelay(void) {
  _delay_us(BIT_DUR);
  __builtin_avr_nops(BIT_CAL * 2);
}
void bitLow(void) {
  PORTA.OUTCLR = UPDI_TARGET_BM;
  bitDelay();
}

void bitHigh(void) {
  PORTA.OUTSET = UPDI_TARGET_BM;
  bitDelay();
}

void updiWriteFrame(uint8_t databyte, uint8_t parity) {
  bitLow(); // start
  for (uint8_t i = 8; i != 0; i--) {
    (databyte & 0x01) ? PORTA.OUTSET = UPDI_TARGET_BM : PORTA.OUTCLR = UPDI_TARGET_BM;
    databyte >>= 1 ;
    _delay_us(BIT_DUR);
    __builtin_avr_nops(BIT_CAL);
  }
  (parity) ? bitHigh() : bitLow(); // parity
  bitHigh(); // stop
  bitHigh(); // stop
}

void updiWriteKey(void) {
  updiWriteFrame(0x55, 0);
  updiWriteFrame(0xE0, 1);
  updiWriteFrame(0x20, 1);
  updiWriteFrame(0x67, 1);
  updiWriteFrame(0x6F, 0);
  updiWriteFrame(0x72, 0);
  updiWriteFrame(0x50, 0);
  updiWriteFrame(0x4D, 0);
  updiWriteFrame(0x56, 0);
  updiWriteFrame(0x4E, 0);
}

void updiTriState(void) {
  PORTA.DIRCLR = UPDI_TARGET_BM; // UPDI rx enable
  PORTA.UPDI_TARGET_CTRL &= ~PORT_PULLUPEN_bm; // UPDI pullup disabled
}

void updiHigh(void) {
  PORTA.OUTSET = UPDI_TARGET_BM; // high
  PORTA.DIRSET = UPDI_TARGET_BM; // UPDI tx enable
  _delay_us(20);
}

void updiIdle(void) {
  updiHigh();
  updiTriState();
  _delay_us(1500);
  updiHigh();
}

void updiTrigger(void) {
  // Release UPDI Reset and initiate UPDI Enable by driving low (0.7µs) then tri-state
  PORTA.DIRSET = UPDI_TARGET_BM; // UPDI tx enable
  PORTA.OUTSET = UPDI_TARGET_BM; // high
  PORTA.OUTCLR = UPDI_TARGET_BM; // low
  __builtin_avr_nops(4);
  PORTA.UPDI_TARGET_CTRL &= ~PORT_PULLUPEN_bm; // UPDI pullup disabled
  PORTA.DIRCLR = UPDI_TARGET_BM; // UPDI rx enable (tri-state)
  _delay_us(1500);  // tri-state duration after UPDI Enable trigger pulse
}

void updiEnable(void) {
  updiTrigger();
  updiHigh();
  updiWriteKey();
  updiIdle();
  updiWriteFrame(0x55, 0);
  updiHigh();
  updiTriState();
  _delay_us(1500);
  PORTA.OUTCLR = UPDI_ENABLE_BM;
}

void pulseHV(void) {
  PORTA.OUTSET = UPDI_ENABLE_BM; // start UPDI enable sequence
  _delay_ms(1);                  // initial delay after startup
  PORTA.OUTSET = HV_ENABLE_BM;   // set HV pulse
  _delay_us(35);                 // HV pulse width
  PORTA.OUTCLR = HV_ENABLE_BM;   // clear HV pulse
  _delay_ms(49);                 // tri-state duration after HV pulse
}

void powerCycle(void) {
  PORTA.OUTCLR = POWER_SWITCH_ENABLE_BM; // clear power
  _delay_ms(200);
  PORTA.OUTSET = POWER_SWITCH_ENABLE_BM; // set power
}

uint8_t checkSignal() {
  if (PORTA.IN & UPDIFY_PUSHBUTTON_BM) {  // high
    if (updifyPressed) {
      updifyPressed = false;
      return 0;
    }
    if (highCount < HIGH_COUNT_VAL) highCount++;
    lowCount = 0;
    lastState = true;
  }
  else {  // low
    if (lowCount < LOW_COUNT_VAL) lowCount++;
    highCount = 0;
    lastState = false;
  }
  if (lowCount == LOW_COUNT_VAL) {
    lowCount++;
    updifyPressed = true;
    stableCount = 0;
  }
  if (highCount == HIGH_COUNT_VAL) {
    highCount++;
    stableCount++;
    return 1;
  }
  if (stableCount == 2) {
    stableCount++;
    return 2;
  }
  return 0xFF;
}
