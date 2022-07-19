// cap_test: Find the threshold of the photo resistor sensor

#define PHOTO_RESISTOR_PIN 32

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // periodically read value
  Serial.println(analogRead(PHOTO_RESISTOR_PIN));
  delay(500);
}
