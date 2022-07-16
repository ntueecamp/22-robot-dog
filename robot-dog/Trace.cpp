#include "Trace.h"

Trace::Trace() {
}

Trace::~Trace() {
}

void Trace::init() {
    radar.init();
    leg.initialize();
}

void Trace::Move() {
#ifdef MIN_VEL_TEST
    leg.write(MIN_VEL);
#else
    // base velocity
    // delta velocity (velocity difference between the left/right wheels)
    // minimum distance from the owner to the dog
    // direction presented by binary number
    double baseVel, deltaVel, leftVel = 0, rightVel = 0, dis;
    int dir;

    // make the radar to rotation for a period and the ultra-sonic to detect
    radar.radarRotation();

    // get min dis from the dog
    dis = radar.getDis();

    // compute base velocity according to the min distance
    // to make the base velocity be negative to move backward when the owner is too close
    // to make the base velocity be smaller then 1.0
    baseVel = (dis - THRESHOLD) / _THRESHOLD;

    // kp for moving forward and backward
    if (baseVel > 0)
        baseVel *= KP_FORWARD;
    else
        baseVel *= KP_BACKWARD;

    // get the direction of the owner
    dir = radar.getDir();

    // if the dog fails to detect its owner , the dog will stop
    // (base velocity equals to zero, and delta velocity equals to zero)
    // and wait for the next detection
    if (dir == 0) {
        baseVel = 0;
    }

    // compute for its delta velocity by p control according to the direction
    deltaVel = ((dir & 16) >> 4) * LL + ((dir & 8) >> 3) * L + ((dir & 4) >> 2) * C + ((dir & 2) >> 1) * R + (dir & 1) * RR;

    // 1. avoid one of the wheel not reverse rotation
    // 2. to make the dog to at least move forward when turning
    //    both side of the wheel need to have a lower bound velocity in absolute value
    //    the lower bound will be +/- MIN_VEL
    if (baseVel > 0) {
        if (baseVel + deltaVel > 0) leftVel = baseVel + deltaVel;
        if (baseVel - deltaVel > 0) rightVel = baseVel - deltaVel;
        leftVel = max(leftVel, MIN_VEL);
        rightVel = max(rightVel, MIN_VEL);
    } else if (baseVel < 0) {
        if (baseVel + deltaVel < 0) leftVel = baseVel + deltaVel;
        if (baseVel - deltaVel < 0) rightVel = baseVel - deltaVel;
        leftVel = min(leftVel, -MIN_VEL);
        rightVel = min(rightVel, -MIN_VEL);
    }
    leg.write(leftVel, rightVel);
#endif /* MIN_VEL_TEST */
}

void Trace::Stop() {
    radar.resetRadar();
    leg.write(0, 0);
}
