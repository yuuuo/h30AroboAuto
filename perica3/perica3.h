#pragma once
#include "mbed.h"

enum frequency {
    F0 , F1 , F8 , F64 , F256 , F1024
};
enum channel_ {
     ALL = 16
};
class PERICA
{
private:
    I2C *i2c;
    char ADDR;
    bool brake;
    bool isEnc;
    bool channel;
    long map(long x, long in_min, long in_max, long out_min, long out_max);
    char configData;
    char Div;
    int minSpd;
    int maxSpd;
public:
    PERICA(I2C *i, char addr);
    //bool init(char toMD , bool rvs = false , bool omni = true, bool sound  = false , frequency freq = F256, int channel_ = 0);
    bool motor();
    bool motor(char sendChannel,int verocity);
    bool RawMotor(char sendChannel,int verocity);
    bool wait(int waitT);
    void set(bool isEnc_ = false,bool brake_ = false,int channel_ = 0);
    bool SystemReset(char sendChannel);
    bool ConfigReset(char sendChannel);
    bool DscOn(char sendChannel,bool mode);
    bool OutputFlip(char sendChannel,bool mode);
    bool DscPortOpen(char sendChannel,bool mode);
    bool SoundOn(char sendChannel,bool mode);
    bool SetDivider(char sendChannel,char pulse);
    void SetSpeedLimit(int min, int max){ minSpd = min; maxSpd = max; }
    int motorOut[16];
    int power[16];

};