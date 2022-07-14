#include "Radar.h"

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "PulseIn.h"

Radar::Radar() {
    for (int i = 0; i < 5; i++) 
        Dis[i] = 0;
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
}

double Radar::getDis() {
    return minDis;
}

short Radar::getDir() {
    return direction;
}

void Radar::radarRotation() {
    for (int i = 0; i < 5; i++)
        Dis[i] = 0;
    
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

        if (Dis[pos]) {
            double temp = disMeasuring();
            if(temp >= 1)
              if (Dis[pos] <= _THRESHOLD && temp <= _THRESHOLD)
                  Dis[pos] = (Dis[pos] + temp) / 2;
              else
                  Dis[pos] = min(Dis[pos], temp);
        } else {
            double temp = disMeasuring();
            if(temp < 1)
              Dis[pos] = 2 * _THRESHOLD;
            else Dis[pos] = temp;
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

    digitalWrite(TRIGPIN, HIGH);  // 10 microseconds HIGH for trig pin to start ultrasonic sensing
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);  

    taskEXIT_CRITICAL(&mux);

    duration = pulseInThreadSafe(ECHOPIN, HIGH, 1000000 * _THRESHOLD/340 * 4);  // duration for HIGH on echo pin
    vTaskDelayUntil(&entryTime, ULTRASONIC_TIMEOUT / portTICK_PERIOD_MS);

    cm = (duration / 2) / 29.1;  // calculate distance based on duration
    return cm;
}

void Radar::calculate() {
    direction = 0;
    minDis = _THRESHOLD;
    for (int i = 0; i < 5; i++) {
        if (Dis[i] <= minDis) {
            minDis = Dis[i];
            direction = (1 << i);
        }
    }
}
