#include "wasa.h"

String dataLogFile = "/data_log.txt";

ESP32Timer dataTimer(0);

volatile uint32_t accumulator = 0;
volatile uint32_t storedData = 0;
volatile uint8_t readCount = 0;
volatile bool shouldWrite = false;

bool IRAM_ATTR dataTimerHandler(void* timerNo) {
    uint8_t val = digitalRead(REED_PIN);

    accumulator = accumulator << 1;
    accumulator = accumulator | val;
    readCount++;

    if (readCount >= SINGLE_WRITE_COUNT) {
        storedData = accumulator;

        accumulator = 0;
        readCount = 0;
        shouldWrite = true;
    }

    return true;
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);

    Serial.println("Setup: WASA Data logging....");

    pinMode(REED_PIN, INPUT_PULLUP);

    delay(500);
    initSDCard();
    delay(500);

    dataTimer.attachInterruptInterval(TIMER_INTERRUPT_WIDTH * 1000, dataTimerHandler);
}

bool sdOk = false;

void loop() {
    delay(25);

    if (shouldWrite) {
        shouldWrite = false;


        auto currentMills = millis();
        log(currentMills, storedData);
        Serial.print(currentMills);
        Serial.print(",");
        Serial.print(storedData);
        Serial.println();

    }
}

void log(unsigned long currentMillis, uint32_t data) {
    if (!sdOk) return;

    File f = SD.open(dataLogFile, FILE_APPEND);
    f.print(currentMillis);
    f.print(",");
    f.print(data);
    f.println();
    f.close();
}

void initSDCard() {
    if (!SD.begin(SDCARD_PIN)) {
        Serial.println("SD Card failed!");
        return;
    }

    sdOk = true;
    Serial.println("SD Card OK.");

    File f = SD.open(dataLogFile, FILE_APPEND);
    f.print("#Begin data collection, format millis,value[newline]. Where millis is the current value of millis() function and value is a integer that represent bitmasks of ");
    f.print(SINGLE_WRITE_COUNT);
    f.print(" sample (least significant bit is recent) those are taken periodically each ");
    f.print(TIMER_INTERRUPT_WIDTH);
    f.print("ms, so each value encode ");
    f.print((SINGLE_WRITE_COUNT*TIMER_INTERRUPT_WIDTH)/1000);
    f.println(" seconds.");
    f.close();
}
