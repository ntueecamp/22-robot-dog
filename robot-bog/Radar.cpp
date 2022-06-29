#include "Radar.h"

Radar::Radar() {
    for (int i = 0; i < 5; i++) {
        preDis[i] = 0;
        curDis[i] = 0;
    }
    angle = CENTERANGLE;
    deltaAngle = ROTATEANGLE;
}

void Radar::init() {
    // pin setup
    pinMode(TRIGPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);
    servo.attach(RADARSERVOPIN);

    // radar servo initialize to starting direction
    servo.write(CENTERANGLE);
    delay(RADARDELAYTIME);
}

int Radar::getDis() {
    return meanDis;
}

short Radar::getDir() {
    return direction;
}

void Radar::radarRotation() {
    for (int i = 0; i < 5; i++) {
        preDis[i] = curDis[i];
        curDis[i] = 0;
    }
    for (int i = 0; i < 9; i++) {
        servo.write(angle);
        delay(RADARDELAYTIME);

        if (curDis[(angle - 30) / 30])
            curDis[(angle - 30) / 30] = (curDis[(angle - 30) / 30] + disMeasuring()) / 2;
        else {
            curDis[(angle - 30) / 30] = disMeasuring();
        }

        if (angle >= CENTERANGLE + ROTATEANGLE * 2 || angle <= CENTERANGLE - ROTATEANGLE * 2)
            deltaAngle -= (deltaAngle + deltaAngle);
        angle += deltaAngle;
    }
    calculate();
}

int Radar::disMeasuring() {
    int duration, cm;
    digitalWrite(TRIGPIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIGPIN, HIGH);  // 給 Trig 高電位，持續 10微秒
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);  // 讀取 echo 的電位
    int s = millis();
    duration = pulseIn(ECHOPIN, HIGH);  // 收到高電位時的時間
    while (millis() - s >= 150) {
    }
    cm = (duration / 2) / 29.1;  // 將時間換算成距離 cm
    return cm;
}

void Radar::calculate() {
    short times = 0;
    meanDis = 0;
    direction = 0;
    for (int i = 0; i < 5; i++) {
        if (preDis[i] <= THRESHOLD && curDis[i] <= THRESHOLD) {
            times++;
            direction += 1 << (4 - i);
            meanDis += curDis[i];
        }
    }
    if (times > 0)
        meanDis /= (float)times;
}
