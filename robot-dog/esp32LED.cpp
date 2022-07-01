#include "esp32LED.h"

eps32LED::eps32LED() {
}
void eps32LED::initialize() {
    ledMatrix.init();
    ledMatrix.setText(text);
}
void eps32LED::set_text(char input_text[10]) {
    for (int i = 0;i <= 9;i++) {
        text[i] = input_text[i];
    }
    ledMatrix.setText(text);
}
void eps32LED::set_pattern(byte input_pattern[9]) {
    for (int i = 0;i <= 8;i++) {
        pattern[i] = input_pattern[i];
    }
}
void eps32LED::print_text() {
    ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(50);
}
void eps32LED::print_pattern() {
    ledMatrix.clear();
    for (int i = 0;i <= 7;i++) {
        ledMatrix.setColumn(i, pattern[i]);
    }
    ledMatrix.commit();
    delay(50);
}
