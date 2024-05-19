#include "wasa.h"

String dataLogFile = "/data_log.csv";

ESP32Timer dataTimer(0);
volatile uint64_t accumulator = 0;
volatile uint64_t storedData = 0;
volatile uint8_t readCount = 0;
volatile bool shouldWrite = false;
bool IRAM_ATTR dataTimerHandler(void* timerNo) {
    uint8_t val = digitalRead(REED_PIN);
    // Serial.print(val);
    digitalWrite(32, val);

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
    pinMode(32, OUTPUT);

    delay(500);
    initSDCard();
    delay(500);

    dataTimer.attachInterruptInterval(1000 * 1000, dataTimerHandler);
}

bool sdOk = false;

void loop() {
    delay(50);

    if (shouldWrite) {
        shouldWrite = false;

        String line = "";
        uint8_t cnt = SINGLE_WRITE_COUNT;
        while (cnt--) {
            line += storedData & 1 ? "1" : "0";
            storedData = storedData >> 1;
        }
        Serial.println(line);
    }
}

void log(String message) {
    if (!sdOk) return;

    File f = SD.open(dataLogFile, FILE_APPEND);
    f.println(message);
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
    f.println("Begin data collect");
    f.close();
}
