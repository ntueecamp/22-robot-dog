// switch_test: Understand pullup/pulldown resistor and bounce

#define LIMIT_SWITCH_PIN 12

int count = 0;

void IRAM_ATTR onLimitSwitchISR()
{
  count++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LIMIT_SWITCH_PIN, INPUT);
  // pinMode(LIMIT_SWITCH_PIN, INPUT_PULLDOWN);

  // Method 2: interrupt (pt. 1)
  // attachInterrupt(LIMIT_SWITCH_PIN, onLimitSwitchISR, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Method 1: periodically read value
  Serial.println(digitalRead(LIMIT_SWITCH_PIN));
  delay(100);

  // Method 2: interrupt (pt. 2)
  // Serial.println(count);
  // count = 0;
  // delay(1000);
}
