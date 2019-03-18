#include "Serial6050.h"

Serial6050::Serial6050(PinName tx, PinName rx, PinName resetPin)
    : serial(tx, rx, 38400), rst(resetPin), Deg(0), Bias(0)
{
}

void Serial6050::init()
{
    rst = 0;
    wait(0.2);
    rst = 1;
    while(1) {
        serial.putc(100);
        if(serial.readable()) {
            if(serial.getc()) break;
        }
    }
    serial.attach(this, &Serial6050::intReceive, RawSerial::RxIrq);
    wait(0.1);
    reset();
}

void Serial6050::intReceive()
{
    data = serial.getc();
    static uint8_t old = data;

    if(data > old && data - old > 127) {
        Bias -= 255;
    } else if(data < old && old - data > 127) {
        Bias += 255;
    } else {
        
    }
    Deg = data + Bias;
    old = data;
}

void Serial6050::reset()
{
    resetValue = Deg;
}



float Serial6050::read()
{
    return (Deg - resetValue) / 10.0;
}