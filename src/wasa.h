#ifndef __WASA_H__
#define __WASA_H__

#include <Arduino.h>
#include <Wire.h>

#include "ESP32TimerInterrupt.h"
#include "FS.h"
#include "SD.h"

#define REED_PIN 22
#define SDCARD_PIN 5

#define SINGLE_WRITE_COUNT 60

void initSDCard();

#endif
