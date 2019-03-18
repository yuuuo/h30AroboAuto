#pragma once
#include "mbed.h"

enum PS4Button {
    SQUARE, CROSS, CIRCLE, TRIANGLE, LEFT, RIGHT, UP,  //0~6
    R3, L3, OPTIONS, SHARE, R1, L1, DOWN,              //7~13
    LeftHatX, LeftHatY, RightHatX, RightHatY, L2, R2   //14~19
};

class PS4Serial
{
private:
    RawSerial WirelessSerial;
    volatile char ControllerData[8];
    volatile char ControllerData_OLD[8];
    volatile char Receive[8];
    volatile int rp;
    volatile int check;
    void WirelessRecive();
    Timer t;
public:
    PS4Serial(PinName tx, PinName rx);
    int getButtonPress(PS4Button button);
    int getButtonClick(PS4Button button,bool mode );
    bool connected();
    void update();
    //void ClickListener(PS4Button ClickButton);
};
