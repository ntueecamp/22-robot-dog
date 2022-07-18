/**
 * @file Interact.h
 * @author ntueecamp 2022 (FrSh28)
 * @brief Header file of functions about interaction inputs (CapTouch, LimitSwitch and PhotoResistor)
 *        and outputs (Sound and LED) of the robot dog.
 * @date 2022-06-30
 *
 */

#ifndef INTERACT_H
#define INTERACT_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Init function and ISR for capacitance touch sensor
 * 
 * @param pin The pin to be attached to the capacitance touch sensor
 *            Available pins: (0), 2, 4, 12, 13, 14, 15, 27, 32, 33
 * @param threshold Triggered low, i.e. the bigger the threshold, the more sensible is the touch (16-bits)
 * 
 * Call `touchRead(pin)` to get the raw measurement of the capacitance touch sensor,
 * and use as a reference to determine the threshold
 */
void IRAM_ATTR onCapTouchISR();
int initCapTouch(const uint8_t& pin, const uint16_t& threshold);

/**
 * @brief Init function and ISR for limit switch
 *
 * @param pin The pin to be attached to the limit switch
 * @param triggerMode How interrupt is triggered, either RISING or FALLING
 */
void IRAM_ATTR onLimitSwitchISR();
int initLimitSwitch(const uint8_t& pin, const int& triggerMode);

typedef struct
{
    uint8_t pin;
    uint16_t threshold;
    uint32_t period;
    TaskHandle_t callingTask;
} photo_R_config_t;
/**
 * @brief Init function and periodic handler for photo resistor
 *
 * @param pin The pin to be attached to the photo resistor
 *            Available pins: 32, 33, 34, 35, 36, 37, 38, 39
 * @param threshold Triggered low, i.e. the bigger the threshold, the more sensible is the touch (12-bits)
 * @param period The period between each analogRead(), in millisecond
 * 
 * Call `analogRead(pin)` to get the raw mmeasurement of the photo resistor,
 * and use as a reference to determine the threshold
 * 
 * This feature CANNOT be made an interrupt since analogRead() is too slow
 */
void handlePhotoResistor(void* argv);
TaskHandle_t initPhotoResistor(const uint8_t& pin, const uint16_t& threshold, const uint32_t& period = 1000);

/**
 * @brief Init function and infinite running handler related to sound
 *
 * @param pin The pin to output the sound (by DAC), GPIO 25 or 26
 */
void handleSound(void* argv);
TaskHandle_t initSound(const uint8_t& pin);

/**
 * @brief Init function and infinite running handler related to LED
 *
 * @param sck  Serial Clock pin
 * @param miso Master In, Slave Out pin
 * @param mosi Master Out, Slave In pin
 * @param cs   Chip Select pin
 */
void handleLED(void* argv);
TaskHandle_t initLED(const uint8_t& sck, const uint8_t& miso, const uint8_t& mosi, const uint8_t& cs);

#endif // INTERACT_H
