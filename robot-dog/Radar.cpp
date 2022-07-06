#include "Radar.h"

#include <Arduino.h>
#include "PulseIn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Radar::Radar() {
    for (int i = 0; i < 5; i++) {
        preDis[i] = 0;
        curDis[i] = 0;
    }
    angle = CENTERANGLE;
    deltaAngle = LEFT_HALF_ROTATEANGLE;
}

void Radar::init() {
    // pin setup
    pinMode(TRIGPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);
    servo.initialize(DEFAULT_NECK_CHANNEL, RADARSERVOPIN, NECKSERVO_MIN_US, NECKSERVO_MAX_US);

    // radar servo initialize to starting direction
    servo.write(CENTERANGLE);
    // delay(RADARDELAYTIME);
}

double Radar::getDis() {
    return minDis;
}

short Radar::getDir() {
    return direction;
}

void Radar::radarRotation() {
    for (int i = 0; i < 5; i++) {
        preDis[i] = curDis[i];
        curDis[i] = 0;
    }
    for (int i = 0; i < 8; i++) {
        servo.write(angle);
        vTaskDelay(RADARDELAYTIME / portTICK_PERIOD_MS);

        int pos;
        if (i == 0 || i == 4) {  // center
            pos = 2;
        } else if (i == 1 || i == 3) {  // left
            pos = 3;
        } else if (i == 2) {  // left most
            pos = 4;
        } else if (i == 5 || i == 7) {  // right
            pos = 1;
        } else if (i == 6) {  // right most
            pos = 0;
        }

        if (curDis[pos]) {
            double temp = disMeasuring();
            if (curDis[pos] <= THRESHOLD && temp <= THRESHOLD)
                curDis[pos] = (curDis[pos] + temp) / 2;
            else
                curDis[pos] = min(curDis[pos], temp);

        } else {
            curDis[pos] = disMeasuring();
        }

        if (i < 2) {
            angle += LEFT_HALF_ROTATEANGLE;
        } else if (i < 4) {
            angle -= LEFT_HALF_ROTATEANGLE;
        } else if (i < 6) {
            angle -= RIGHT_HALF_ROTATEANGLE;
        } else {
            angle += RIGHT_HALF_ROTATEANGLE;
        }
    }
    calculate();
}

double Radar::disMeasuring() {
    static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    TickType_t entryTime = xTaskGetTickCount();

    int duration;
    double cm;

    digitalWrite(TRIGPIN, LOW);
    delayMicroseconds(5);

    taskENTER_CRITICAL(&mux);
    
    digitalWrite(TRIGPIN, HIGH);  // 給 Trig 高電位，持續 10微秒
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);  // 讀取 echo 的電位

    taskEXIT_CRITICAL(&mux);

    duration = pulseInThreadSafe(ECHOPIN, HIGH, 1000 * 3 * 4);  // 收到高電位時的時間
    vTaskDelayUntil(&entryTime, ULTRASONIC_TIMEOUT / portTICK_PERIOD_MS)

        cm = (duration / 2) / 29.1;  // 將時間換算成距離 cm
    return cm;
}

void Radar::calculate() {
    int times = 0;
    direction = 0;
    minDis = THRESHOLD;
    for (int i = 0; i < 5; i++) {
        if (preDis[i] <= THRESHOLD && curDis[i] <= THRESHOLD) {
            times += 1;
            if (curDis[i] <= minDis) {
                minDis = curDis[i];
                direction = (1 << i);
            }
        }
    }
}
