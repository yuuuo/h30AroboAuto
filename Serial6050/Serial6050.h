#pragma once
#include "mbed.h"

#define ERROR -10000

enum axis {
    YAW,
    PITCH,
    ROLL
};

class Serial6050
{
private:
    RawSerial serial;
    DigitalOut rst;
    int resetValue;
    int Bias;
    void intReceive();
public:
    Serial6050(PinName tx, PinName rx, PinName reset);
    void init();
    void reset();
    volatile uint8_t data;
    volatile int Deg;
    float read();
};