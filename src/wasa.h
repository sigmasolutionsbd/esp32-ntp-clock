#ifndef __WASA_H__
#define __WASA_H__

#include <Arduino.h>
#include <Wire.h>

#include "ESP32TimerInterrupt.h"
#include "FS.h"
#include "SD.h"

#define REED_PIN 34
#define SDCARD_PIN 5

#define TIMER_INTERRUPT_WIDTH 200
#define SINGLE_WRITE_COUNT 30

void initSDCard();
void log(unsigned long currentMillis, uint32_t data);

#endif
