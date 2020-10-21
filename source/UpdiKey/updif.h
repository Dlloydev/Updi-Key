/*
   updif.h
   Created: 10-21-2020
   Author: DWL
*/

#include <avr/io.h>

// UPDI pin for target communication
#define UPDI_TARGET_BM PIN3_bm
#define UPDI_TARGET_CTRL PIN3CTRL

// Remaining pin control and bit masks
#define UPDIFY_PUSHBUTTON_BM PIN2_bm
#define UPDIFY_PUSHBUTTON_CTRL PIN2CTRL
#define POWER_SWITCH_ENABLE_BM PIN6_bm
#define HV_ENABLE_BM PIN1_bm
#define UPDI_ENABLE_BM PIN7_bm

// Sets UPDI baudrate to 10kHz
#define BIT_DUR 99 // µs delay
#define BIT_CAL 6 // nops delay

#define LOW_COUNT_VAL 0x7FF // button press (low)
#define HIGH_COUNT_VAL 0x3FFF // signal stable (high)

void bitDelay(void);
void bitHigh(void);
void bitLow(void);
void updiWriteFrame(uint8_t databyte, uint8_t parity);
void updiWriteKey(void);
void updiKeyInit(void);
void pulseHV(void);
void updiTriState(void);
void updiHigh(void);
void updiIdle(void);
void updiTrigger(void);
void updiEnable(void);
void powerCycle(void);
uint8_t checkSignal();
