#include "PS4Serial.h"

PS4Serial::PS4Serial(PinName tx, PinName rx) : WirelessSerial(tx, rx, 115200), rp(0), check(0)
{
    for(int i = 0; i < 8; i++) ControllerData[i] = 2 <= i && i <= 5 ? 127 : 0;
    WirelessSerial.attach(this, &PS4Serial::WirelessRecive, RawSerial::RxIrq);
}

void PS4Serial::WirelessRecive()
{
    t.stop();
    t.reset();
    char data = WirelessSerial.getc();
    if(data == 0xff) {
        rp = 0;
        check = 0;
    } else if(rp >= 8) {
        if((check & 0x7f) == data) {
            for (int i = 0; i < 8; i++) {
                ControllerData[i] = Receive[i];
            }

        }
    } else {
        Receive[rp] = data;
        rp++;
        check += data;
    }
    t.start();
}

int PS4Serial::getButtonPress(PS4Button button)
{
    if(button < 7) {
        return (ControllerData[0] >> button) & 1;
    } else if(button < 14) {
        return (ControllerData[1] >> (button - 7)) & 1;
    } else {
        return ControllerData[button - 12];
    }
}
int PS4Serial::getButtonClick( PS4Button button, bool mode )
{
    //mode:0 -> Rising
    //mode:1 -> Falling

    if(button < 7) {
        return !mode * (((ControllerData[0] >> button) & 1) * !((ControllerData_OLD[0] >> button) & 1))
               + mode * ((!(ControllerData[0] >> button) & 1) * ((ControllerData_OLD[0] >> button) & 1));
    } else if(button < 14) {
        return !mode * (((ControllerData[1] >> (button - 7)) & 1) * !((ControllerData_OLD[1] >> (button - 7)) & 1))
               + mode * ((!((ControllerData[1] >> (button - 7)) & 1) * ((ControllerData_OLD[1] >> (button - 7)) & 1)));
    } else if( (14<=button) && (button<=19) ) {
        return !mode*((ControllerData[button - 12]>127)*!(ControllerData_OLD[button - 12]>127))
               + mode*(!(ControllerData[button - 12]>127)*(ControllerData_OLD[button - 12]>127));

    } else {
        return 0;
    }
}

bool PS4Serial::connected()
{
    return t.read_ms() < 500;
}

void PS4Serial::update()
{
    for (int i = 0; i < 8; i++)
        ControllerData_OLD[i] = ControllerData[i];

    wait_ms(1);
}