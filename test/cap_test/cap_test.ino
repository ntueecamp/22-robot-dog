// cap_test: Find the threshold of the cap sensor (touch sensor)

#define CAP_TOUCH_PIN 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Method 1: periodically read value
  Serial.println(touchRead(CAP_TOUCH_PIN));
  delay(100);
}
