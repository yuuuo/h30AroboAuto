#include "perica3.h"

PERICA::PERICA(I2C *i, char addr)
{
    i2c = i;
    ADDR = addr;
    brake = isEnc = channel = false;
    configData = 0;
    Div = 0;
    maxSpd = 240;
    minSpd = 0;
    for (int i = 0 ; i < 16; i++)  {
        motorOut[i] = 0;
        power[i] = 0;
    }
}

/*bool PERICA::init(char toMD,bool rvs , bool omni , bool sound , frequency freq , int channel_)
{
    char send[3];
    send[0] = 0x90 | channel;
    send[1] = omni << 4 | rvs << 2 | sound;
    send[2] = freq << 4 | channel_;
    channel = channel_;
    return i2c->write(ADDR, send, 3);
}*/

void PERICA::set(bool isEnc_,bool brake_,int channel_ )
{
    brake = brake_;
    isEnc = isEnc_;
    channel = channel_;

}

long PERICA::map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool PERICA::motor()
{
    bool mem_ = 1;
    for (int i = 0; i < 16; i++) {
        mem_ = motor(i,power[i]);
        wait_us(10);
    }
    return mem_;
}

bool PERICA::motor(char sendChannel,int verocity)
{
    char v = abs(verocity);
    char direction;
    if (v > 0) {
        if (v > 255) v = 255;
        v = map(v, 0, 255, minSpd, maxSpd);
        direction = verocity < 0;
    } else {
        direction = (0b10 | !brake);
    }
    char send[3] = {0x80 | sendChannel , (isEnc << 2) | direction ,v >> 1};
    motorOut[sendChannel] = (verocity < 0) ? v * -1 : v;
    return i2c->write(ADDR, send, 3);
}

bool PERICA::RawMotor(char sendChannel,int verocity)
{
    char direction;
    if (verocity > 0) {
        direction = 0b00;
        if (verocity > 255) verocity = 255;
    } else if (verocity < 0) {
        direction = 0b01;
        if (verocity < -255) verocity = -255;
        verocity *= -1;
    } else direction = 0b10 | !brake;
    char send[3] = {0x80 | sendChannel , (isEnc << 2) | direction, verocity};
    return i2c->write(ADDR, send, 3);
}



bool PERICA::wait(int waitT)
{
    if (waitT > 0x3FFF) waitT = 0x3FFF;
    char send[3];
    send[0] = 0xE0 | channel;
    send[1] = waitT >> 7;
    send[2] = waitT & 0x7F;
    return i2c->write(ADDR, send, 3);
}



bool PERICA::SystemReset(char sendChannel)  //7bit
{
    configData = 0;
    char send[3] = {0x90 | sendChannel,0x40,0x00};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::ConfigReset(char sendChannel)  //6bit
{
    configData = 0;
    char send[3] = {0x90 | sendChannel,0x20,0x00};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::DscOn(char sendChannel,bool mode = 1)  //4bit 1:on 0:off
{
    configData = (configData & 0b11110111) | (mode << 3);
    char send[4] = {0x90 | sendChannel,configData,Div};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::OutputFlip(char sendChannel,bool mode = 1)  //3bit 1:on 0:off
{
    configData = (configData & 0b11111011) | (mode << 2);
    char send[3] = {0x90 | sendChannel,configData,Div};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::DscPortOpen(char sendChannel,bool mode = 1)   //2bit 1:on 0:off
{
    configData = (configData & 0b11111101) | (mode << 1);
    char send[3] = {0x90 | sendChannel,configData,Div};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::SoundOn(char sendChannel,bool mode = 1) //1bit 1:on 0:off
{
    configData = (configData & 0b11111110) | mode;
    char send[3] = {0x90 | sendChannel,configData,Div};
    return i2c->write(ADDR, send, 3);
}

bool PERICA::SetDivider(char sendChannel,char pulse = 0)//0-5
{
    Div = pulse << 4;
    char send[4] = {0x90 | sendChannel,configData,Div};
    return i2c->write(ADDR, send, 3);
}
