// cap_test: Find the threshold of the cap sensor (touch sensor)

#define CAP_TOUCH_PIN 4

int count = 0;

void IRAM_ATTR onCapISR()
{
  count++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Method 2: interrupt (pt. 1)
  // touchAttachInterrupt(CAP_TOUCH_PIN, onCapISR, 50);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Method 1: periodically read value
  Serial.println(touchRead(CAP_TOUCH_PIN));
  delay(100);

  // Method 2: interrupt (pt. 2)
  // Serial.println(count);
  // count = 0;
  // delay(1000);
}
