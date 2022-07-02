#include "PulseIn.h"

#define TRIGPIN 27
#define ECHOPIN 14

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(TRIGPIN, OUTPUT);
    pinMode(ECHOPIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    int duration;
    double cm;

    digitalWrite(TRIGPIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIGPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);
    
    duration = pulseInThreadSafe(ECHOPIN, HIGH);
    cm = (duration / 2) / 29.1;

    Serial.println(cm);

    delay(1000);
}
