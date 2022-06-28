#include "Leg.h"

Leg leg;

void setup() {
  // default pins and ledc channels are set in initialize()
  leg.initialize();
}

void loop() {
  // let robot dog move forward for 2 sec
  leg.write(1.0, 1.0);
  delay(2000);
  // let robot dog move backward for 2 sec
  leg.write(-1.0, -1.0);
  delay(2000);
  // let robot dog rotate clockwise for 2 sec
  leg.write(1.0, -1.0);
  delay(2000);
  // let robot dog rotate counterclockwise for 2 sec
  leg.write(-1.0, 1.0);
  delay(2000);
}
