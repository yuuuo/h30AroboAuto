#include "mbed.h"
#include <string>
#include "QEI.h"
#include "Serial6050.h"
#include "perica3.h"
#include "PS4Serial.h"
#include "VL53L1X.h"
#include "SB1602E.h"
#include "Utility.h"

void ReadLineMainThread()
{
    while(1) {
        for(int i = 0; i < 6; i++) {
            GPIO[i].output();
            GPIO[i] = 1;
            Timer t;
            wait_us(100);
            GPIO[i].input();
            t.start();
            bool flg = false;
            while(GPIO[i]) {
                if(t.read_ms() > 10) {
                    flg = true;
                    break;
                }
            }
            t.stop();
            if(flg) continue;
            Line[i] = t.read_us();
            t.reset();
            wait_ms(1);
        }
    }
}

void MoveForwardTable(int pt)
{
    wait(0.05);
    int dir[2] = {0, 180};
    int Loop = 1;
    float stopDist = 340;
    if(pt == 1) {
        stopDist = 360;
        Loop = round_(SlideVol[2] + 1);
    } else if(pt == 2) {
        stopDist = 220;
        Loop = round_(SlideVol[1] + 1);
    } else if(pt == 3) {
        stopDist = 140;
        Loop = round_(SlideVol[0] + 1);
    } else if(pt == 4) {
        stopDist = 180;
        pt = 3;
    }

    int targetSpd = 50;
    float breakPos = 4000;
    float breakDist = 300;

    Move(5, -90);
    wait(1);
    Move(0, 0);
    float defDeg = mpu.read();
    Enc[1].reset();
    wait(0.5);
    Timer t;
    t.start();
    bitWrite(SV, pt, 1);
    for(int spd = 0; spd <= targetSpd; spd++) {
        if(targetSpd * ((Distance - stopDist) / breakDist) < spd) break;
        Move(spd, 90 + LineEval() * 2);
        for(int i = 0; i < 4; i++)
            Motor[i] -= (mpu.read() - defDeg) * 2;
        wait(0.01);
    }

    while(Distance >= stopDist + breakDist) {
        Move(targetSpd, 90 + LineEval() * 3);
        for(int i = 0; i < 4; i++)
            Motor[i] -= (mpu.read() - defDeg) * 2;
    }

    while(Distance - stopDist >= 10) {
        float spd = targetSpd * ((Distance - stopDist) / breakDist);
        Move(spd, 90 + LineEval() * 4);
        for(int i = 0; i < 4; i++)
            Motor[i] -= (mpu.read() - defDeg) * 2;
    }
    pc.printf("Stop Table\n\r");
    Move(0, 0);

    //うつ
    while(t.read_ms() < 2000);
    Buz.period(1 / 880.0);
    Buz = 0.5;
    wait(1);
    Buz = 0;

    bitWrite(SV, 4, 1);
    t.stop();
    wait(1);
    bitWrite(SV, pt, 0);
    wait(0.5);
    bitWrite(SV, 4, 0);

    for(int i = 0; i < Loop - 1; i++) {
        Move(5, 170);
        wait(0.7);
        Move(0, 0);
        bitWrite(SV, pt, 1);

        wait(0.5);
        doReloadPt = -1;
        wait(2.5);

        Buz.period(1 / 880.0);
        Buz = 0.5;
        wait(1);
        Buz = 0;
        bitWrite(SV, 4, 1);
        wait(1);
        bitWrite(SV, pt, 0);
        wait(0.5);
        bitWrite(SV, 4, 0);
    }

    if(pt == 0) {
        breakDist = 500;
        if(!BoardSW[0]) {
            Move(5, dir[BoardSW[1]]);
            wait(0.8);
            Move(0, 0);

            wait(1);
            stopDist = 150;
            pt = 3;
            targetSpd = 40;
            doReloadPt = -1;
            t.stop();
            t.reset();
            t.start();
            bitWrite(SV, pt, 1);

            while(Distance - stopDist >= 10) {
                float spd = targetSpd * ((Distance - stopDist) / breakDist);
                Move(spd, 90);
                for(int i = 0; i < 4; i++)
                    Motor[i] -= (mpu.read() - defDeg) * 2;
            }
            pc.printf("Stop Table\n\r");
            Move(0, 0);

            //うつ
            while(t.read_ms() < 3000);
            Buz.period(1 / 880.0);
            Buz = 0.5;
            wait(1);
            Buz = 0;
            bitWrite(SV, 4, 1);
            t.stop();
            wait(1);
            bitWrite(SV, pt, 0);
            wait(0.7);
            bitWrite(SV, 4, 0);


            Move(5, dir[!BoardSW[1]]);
            wait(0.8);
            Move(0, 0);

            bitWrite(SV, pt, 1);

            wait(0.5);
            doReloadPt = -1;
            wait(2.5);

            Buz.period(1 / 880.0);
            Buz = 0.5;
            wait(1);
            Buz = 0;
            bitWrite(SV, 4, 1);
            wait(1);
            bitWrite(SV, pt, 0);
            wait(0.5);
            bitWrite(SV, 4, 0);

            Move(5, dir[!BoardSW[1]]);
            wait(0.8);
            Move(0, 0);

            bitWrite(SV, pt, 1);

            wait(0.5);
            doReloadPt = -1;
            wait(2.5);

            Buz.period(1 / 880.0);
            Buz = 0.5;
            wait(1);
            Buz = 0;
            bitWrite(SV, 4, 1);
            wait(1);
            bitWrite(SV, pt, 0);
            wait(0.5);
            bitWrite(SV, 4, 0);
        } else {
            wait(1.5);
            stopDist = 150;
            pt = 3;
            targetSpd = 40;
            doReloadPt = -1;
            t.stop();
            t.reset();
            t.start();
            bitWrite(SV, pt, 1);

            while(Distance - stopDist >= 10) {
                float spd = targetSpd * ((Distance - stopDist) / breakDist);
                Move(spd, 90 + LineEval() * 4);
                for(int i = 0; i < 4; i++)
                    Motor[i] -= (mpu.read() - defDeg) * 2;
            }
            pc.printf("Stop Table\n\r");
            Move(0, 0);

            //うつ
            while(t.read_ms() < 3000);
            Buz.period(1 / 880.0);
            Buz = 0.5;
            wait(1);
            Buz = 0;
            bitWrite(SV, 4, 1);
            t.stop();
            wait(1);
            bitWrite(SV, pt, 0);
            wait(0.5);
            bitWrite(SV, 4, 0);

        }
    }

    //戻る
    for(int spd = 0; spd <= targetSpd; spd++) {
        Move(spd, -90);
        if(targetSpd * (float)(-Enc[1].getPulses() / breakPos) < spd) break;
        for(int i = 0; i < 4; i++)
            Motor[i] -= (mpu.read() - defDeg) * 0;
        wait(0.005);
    }

    t.reset();
    t.start();
    while(-Enc[1].getPulses() > 300 && t.read_ms() < 6000) {
        if(-Enc[1].getPulses() > breakPos) {
            Move(targetSpd, -90);
            for(int i = 0; i < 4; i++)
                Motor[i] -= (mpu.read() - defDeg) * 1;
        } else {
            Move(targetSpd * (float)(-Enc[1].getPulses() / breakPos), -90);
            for(int i = 0; i < 4; i++)
                Motor[i] -= (mpu.read() - defDeg) * 1;
        }
    }

    Move(5, -90);
    wait(1);
    Move(0, 0);
    doReloadPt = -1;
}

void ManualThread()
{
    while(1) {
        if (PS4.connected()) {
            if (doAutomode) {
                printf("AutoMode!!\r\n");
                MotorThread();
            } else {
                PS4.update();

                double Lx = map(constrain(PS4.getButtonPress(LeftHatX), 0, 255),0,255,-255,255);
                double Ly = map(constrain(PS4.getButtonPress(LeftHatY), 0, 255),0,255,-255,255);
                double Rx = map(constrain(PS4.getButtonPress(R2) - PS4.getButtonPress(L2), -255, 255),-255,255,-100,100);
                double Lrad = atan2(-Ly, Lx);
                double Ldeg = Lrad * 180/  M_PI;
                double Ldist = constrain(sqrt((double)(Lx * Lx + Ly * Ly)),0,255);
                if (Ldist >= 30) Ldist = map(Ldist, 30, 255, 1, 255);
                else Ldist = 0;

                Move(Ldist,Ldeg,Rx);

                if(PS4.getButtonClick(CIRCLE, 0)) doReloadPt = 2;
                else if(PS4.getButtonClick(CROSS, 0)) doReloadPt = 3;
                else if(PS4.getButtonClick(SQUARE, 0)) doReloadPt = -1;

                if((PS4.getButtonPress(L1) && PS4.getButtonPress(R1) && PS4.getButtonPress(UP) && PS4.getButtonPress(TRIANGLE)) || !PanelSW[2]) doAutomode = 1;



            }
        }
    }
}

void MotorThread()
{
    Enc[0].reset();
    Enc[1].reset();
    Sign = BoardSW[1] ? -1 : 1;
    int dir[2] = {-6, 183};
    bool once = BoardSW[0];
    Move(0, 0);
    while(PanelSW[0]) {
        while(PanelSW[2]);
        MoveForwardTable(0);
    }

    //1m 4700pls
    while(1) {


        //lcd.printf(0, 1, "WAITING...");
        int targetSpd = 280;
        int dist = 26000;
        float breakPos = 10000;

        targetSpd = 200;
        dist = 14800;
        breakPos = 7000;

        for(int spd = 0; spd <= targetSpd; spd++) {
            Move(spd, dir[BoardSW[1]]);
            wait_us(600);
            wait_us(400);
        }

        while(Sign * Enc[0].getPulses() < dist - 300) {
            if(Sign * Enc[0].getPulses() < dist - breakPos) Move(targetSpd, dir[BoardSW[1]]);
            else Move(targetSpd * ((float)(Sign * -Enc[0].getPulses() + dist) / breakPos), dir[BoardSW[1]]);
        }

        while(!(Line[0] < 1800 || Line[5] < 1800));
        Move(0, 0);

        MoveForwardTable(2);
        
        targetSpd = 50;

        dist = 11100;
        breakPos = 2000;

        //3番め移動
        for(int spd = 0; spd <= targetSpd; spd++) {
            Move(spd, dir[!BoardSW[1]]);
            wait(0.001);
        }

        while(Sign * Enc[0].getPulses() > dist + 200) {
            if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
            else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]]);
        }
        while(!(Line[0] < 1800 || Line[5] < 1800));
        Move(0, 0);

        MoveForwardTable(3);

        dist = -800;
        breakPos = 6000;

        for(int spd = 0; spd <= targetSpd; spd++) {
            Move(spd, dir[!BoardSW[1]]);
            wait(0.001);
        }

        while(Sign * Enc[0].getPulses() > dist + 500) {
            if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
            else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]]);
        }

        Move(5, 90);
        wait(0.1);
        Move(0, 0);
        break;/////////********************************************************************************************************

        //一番奥
        while(PanelSW[2]);
        Sequence = (BoardSW[2] << 1) | BoardSW[3];

        switch(Sequence) {
            case 0:
                //default Value
                break;
            case 1:
                targetSpd = 200;
                dist = 22000;
                breakPos = 8000;
                break;
            case 2:
                targetSpd = 180;
                dist = 17000;
                breakPos = 7000;
                dir[BoardSW[1]] += 1 * -Sign;
                break;
            case 3:
                targetSpd = 100;
                dist = 8000;
                breakPos = 3000;
                dir[BoardSW[1]] += 3 * -Sign;
                break;

        }

        for(int spd = 0; spd <= targetSpd; spd++) {
            Move(spd, dir[BoardSW[1]]);
            wait_us(400);
            wait_us(400);
        }
        while(Sign * Enc[0].getPulses() < dist - 300) {
            if(Sign * Enc[0].getPulses() < dist - breakPos) Move(targetSpd, dir[BoardSW[1]]);
            else Move(targetSpd * ((float)(Sign * -Enc[0].getPulses() + dist) / breakPos), dir[BoardSW[1]]);
        }

        while(!(Line[0] < 1800 || Line[5] < 1800));
        Move(0, 0);

        switch(Sequence) {
            case 0:
                //一番奥前出る
                MoveForwardTable(1);

                if(once) {
                    targetSpd = 180;
                    dist = -1500;
                    breakPos = 10000;
                    break;
                }

                targetSpd = 30;

                //2番め移動
                for(int spd = 0; spd <= targetSpd; spd++) {
                    Move(spd, dir[!BoardSW[1]]);
                    wait(0.001);
                }

                targetSpd = 50;

                dist = 24000;
                breakPos = 2000;

                while(Sign * Enc[0].getPulses() > dist + 200) {
                    if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
                    else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]] + 2 * -Sign);
                }
                while(!(Line[0] < 1800 || Line[5] < 1800));
                Move(0, 0);

            case 1:
                //前出る
                MoveForwardTable(2);

                if(once) {
                    targetSpd = 180;
                    dist = -1500;
                    breakPos = 10000;
                    break;
                }

                targetSpd = 50;

                //3番め移動
                for(int spd = 0; spd <= targetSpd; spd++) {
                    Move(spd, dir[!BoardSW[1]]);
                    wait(0.001);
                }

                targetSpd = 50;

                dist = 19000;
                breakPos = 2000;

                while(Sign * Enc[0].getPulses() > dist + 200) {
                    if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
                    else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]]);
                }
                while(!(Line[0] < 1800 || Line[5] < 1800));
                Move(0, 0);

            case 2:
                MoveForwardTable(3);

                if(once) {
                    targetSpd = 180;
                    dist = -1500;
                    breakPos = 10000;
                    break;
                }

                targetSpd = 80;

                //2段へ移動
                for(int spd = 0; spd <= targetSpd; spd++) {
                    Move(spd, dir[!BoardSW[1]]);
                    wait(0.001);
                }

                dist = 10000;
                breakPos = 4000;

                while(Sign * Enc[0].getPulses() > dist + 200) {
                    if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
                    else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]]);
                }
                while(!(Line[0] < 1800 || Line[5] < 1800));
                Move(0, 0);

            case 3:

                MoveForwardTable(0);

                dist = -1500;
                breakPos = 6000;

            default:
                break;
        }

        for(int spd = 0; spd <= targetSpd; spd++) {
            Move(spd, dir[!BoardSW[1]]);
            wait(0.001);
        }

        while(Sign * Enc[0].getPulses() > dist + 500) {
            if(Sign * Enc[0].getPulses() > dist + breakPos) Move(targetSpd, dir[!BoardSW[1]]);
            else Move(targetSpd * ((float)(Sign * Enc[0].getPulses() - dist) / breakPos), dir[!BoardSW[1]]);
        }

        Move(5, 90);
        wait(0.1);
        Move(0, 0);

    }
    doAutomode = false;
}

// main() runs in its own thread in the OS
int main()
{
    lcd.printf(1, "INIT...");
    Reset = 1;
    wait(0.2);
    Reset = 0;

    Thread buz;
    buz.start(BuzStart);

    WirelessSel = 1;

    for(int i = 0; i < 4; i++) {
        BoardSW[i].mode(PullUp);
        PanelSW[i].mode(PullUp);
    }

    if (vl53.begin() == false) {
        pc.printf("Sensor offline!\n\r");
        return 1;
    }


    //vl53.setDistanceMode(0);

    mpu.init();

    perica.SetSpeedLimit(13, 240);

    Thread th;
    th.start(ReadLineMainThread);

    Thread rel;
    rel.start(ReloadThread);

    Thread sv;
    if(PanelSW[1])
        sv.start(SVThread);
    Thread mt;
    lcd.clear();
    Thread buz1;
    buz1.start(BuzReady);
    LED[0] = 1;
    lcd.printf(0, "READY");
    //mt.start(MotorThread);
    mt.start(ManualThread);
    while (true) {
        static bool R3Flag = true;
        if(PS4.getButtonPress(R3) && R3Flag){
            R3Flag = false;
            ProgEmerStop = !ProgEmerStop;
        } else if(!PS4.getButtonPress(R3)) {
            R3Flag = true;
        }
        
        LED[0] = ProgEmerStop;
        for(int i = 0; i < 4; i++) {
            LED[i + 1] = PanelSW[i];
        }
        wait_us(100);
        perica.motor(0, Motor[0] * ProgEmerStop);
        wait_us(100);
        perica.motor(1, Motor[0] * ProgEmerStop);
        wait_us(100);
        perica.motor(2, Motor[1] * ProgEmerStop);
        wait_us(100);
        perica.motor(3, Motor[2] * ProgEmerStop);
        wait_us(100);
        perica.motor(4, Motor[3] * ProgEmerStop);
        wait_us(100);

        char cmd = SV;
        i2c.write(0x51 << 1, &cmd, 1);
        if(vl53.newDataReady()) {
            int buf = vl53.getDistance();
            if(buf > 1000)
                Distance = 1000;
            else if(buf != 0)
                Distance = buf;
        }


        pc.printf("%d  ** ", ProgEmerStop);
        pc.printf("%5d  ** ", Distance);
        pc.printf("%f  ** ", mpu.read());
        for(int i = 0; i < 6; i++)
            pc.printf("%6d :", Line[i]);

        //for(int i = 0; i < 4; i++)
        //pc.printf("%4d :", Motor[i]);

        //LineEval();
        pc.printf("%6d :", LineEval());

        pc.printf("X:%5d - Y:%5d", Enc[0].getPulses(), Enc[1].getPulses());
        pc.printf("\n\r");

    }
}

