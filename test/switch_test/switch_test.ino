// switch_test: Understand pullup/pulldown resistor and bounce

#define LIMIT_SWITCH_PIN 12

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Method 1: periodically read value
  Serial.println(digitalRead(LIMIT_SWITCH_PIN));
  delay(100);
}
