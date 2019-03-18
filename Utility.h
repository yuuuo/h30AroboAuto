#pragma once

#define JOYBIAS 15
#define M_PI 3.1415926535897932384626433832795

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

QEI Enc[2] = {QEI(PC_6, PC_5, PC_8, 360), QEI(PA_11, PB_12, PA_12, 360)};
RawSerial pc(USBTX, USBRX, 115200);
DigitalOut WirelessSel(PC_4);
Serial6050 mpu(PC_10, PC_11, PC_12);
I2C i2c(D14,D15);
SB1602E lcd(&i2c, "Cerica2 Start");
PERICA perica(&i2c, 0x10 << 1);
VL53L1X vl53(&i2c);
PS4Serial PS4(PA_9, PA_10);
DigitalOut Reset(PC_12);
DigitalOut LED[5] = {DigitalOut(PA_15), DigitalOut(PB_7), DigitalOut(PC_13), DigitalOut(PC_2), DigitalOut(PC_3)};
DigitalIn BoardSW[4] = {DigitalIn(PC_0), DigitalIn(PC_1), DigitalIn(PB_0), DigitalIn(PA_4)};
DigitalInOut GPIO[6] = {DigitalInOut(PB_10), DigitalInOut(PB_13), DigitalInOut(PB_14),
                        DigitalInOut(PB_15), DigitalInOut(PB_1), DigitalInOut(PB_2)
                       };
AnalogIn SlideVol[3] = {AnalogIn(PA_5), AnalogIn(PA_7), AnalogIn(PA_6)};
DigitalIn PanelSW[4] = {DigitalIn(PC_9), DigitalIn(PB_6), DigitalIn(PC_7), DigitalIn(PA_8)};
PwmOut Buz(PA_0);

volatile int Line[6] = {0};
volatile int Motor[4] = {0};
volatile char SV = 0;
volatile int Distance;
volatile int doReloadPt = 0; //-1 : Reload, 0 : None, Pt : LaunchPt
volatile int Sequence = 0;
volatile int Sign = 1;
bool ProgEmerStop = true;
volatile bool doAutomode = false;
volatile int AutomodeCount = 0;

int round_( double x )
{
    return (int)( x < 0.0 ? x-0.5 : x+0.5 );
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int constrain(int x, int a, int b)
{
    if (x < a) return a;
    else if (x > b) return b;
    else return x;
}

void Move(int spd, int dir, double rot = 0)
{
    float rad = dir / 180.0 * M_PI;
    Motor[0] = spd * cos(rad + M_PI / 4) + rot;
    Motor[1] = -spd * cos(rad - M_PI / 4) + rot;
    Motor[2] = -spd * cos(rad + M_PI / 4) + rot;
    Motor[3] = spd * cos(rad - M_PI / 4) + rot;
    //pc.printf("%d  :%d  :%d  :%d\n\r", Motor[0], Motor[1], Motor[2], Motor[3]);
}

int LineEval()
{
    int val = 0;
    static int old = 0;
    for(int i = 0; i < 6; i++) {
        val <<= 1;
        val |= (Line[i] < 2200);

    }
    if(val == 0b00011111) return 0;
    else if(val == 0b00111111) return 0;
    else if(val == 0) {
        return old * 6;
    } else if(val > 0b00011111) {
        int cnt = 0;
        for(int j = 0; j < 6; j++) {
            if(!((val >> j) & 1)) cnt++;
        }
        old = -1;
        return -cnt;
    } else if(val < 0b00011111) {
        int cnt = -1;
        for(int j = 0; j < 6; j++) {
            if((!(val >> j) & 1)) cnt++;
        }
        old = 1;
        return cnt;
    }

    return 0;
}

void Reload()
{

    bitWrite(SV, 7, 1);
    wait(0.8);
    bitWrite(SV, 5, 1);
    wait(1.5);
    bitWrite(SV, 5, 0);
    wait(0.5);
    bitWrite(SV, 7, 0);
    wait(0.5);
    bitWrite(SV, 6, 1);
    wait(1);
    bitWrite(SV, 6, 0);
    wait(1);
    doReloadPt = 0;
}

void ReloadThread()
{
    while(1) {
        if(doReloadPt != 0) {
            if(doReloadPt == -1) Reload();
            else {
                Timer t;
                t.start();
                bitWrite(SV, doReloadPt, 1);
                while(t.read_ms() < 2000);
                Buz.period(1 / 880.0);
                Buz = 0.5;
                wait(1);
                Buz = 0;
                bitWrite(SV, 4, 1);
                t.stop();
                wait(1);
                bitWrite(SV, doReloadPt, 0);
                wait(0.5);
                bitWrite(SV, 4, 0);

                wait(0.5);
                Reload();
                wait(2.5);
            }
        }
    }
}

void ReadLineMainThread();

void MoveForwardTable();

void SVThread()
{
    while(1) {
        if(!PanelSW[3]) {
            Reload();
        } else {
            SV = 0;
            for(int i = 0; i < 4; i++) {
                bitWrite(SV, i, BoardSW[i]);
            }
            bitWrite(SV, 4, !PanelSW[1]);
            wait(0.01);
        }
    }
}

void BuzLThread()
{
    for(int i = 0; i < 30; i++) {
        Buz.period(1 / 880.0);
        Buz = 0.5;
        wait(0.05);
        Buz = 0;
        wait(0.05);
    }
    Buz = 0;

}

void BuzStart()
{
    Buz.period(1 / 2000.0);
    Buz = 0.5;
    wait(0.15);
    Buz.period(1 / 1000.0);
    wait(0.15);
    Buz = 0;
}

void BuzReady()
{
    Buz.period(1 / 2500.0);
    Buz = 0.5;
    wait(0.15);
    Buz = 0;
}
void MotorThread();


