/**
 * @file Interact_Inputs.h
 * @author ntueecamp 2022 (FrSh28)
 * @brief Header file of functions about interactive inputs (CapTouch, LimitSwitch and PhotoResistor) of the robot dog.
 * @date 2022-06-28
 *
 */

#ifndef INTERACT_INPUTS_H
#define INTERACT_INPUTS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define CAP_TOUCH_BIT      (1 << 0)
#define LIMIT_SWITCH_BIT   (1 << 1)
#define PHOTO_RESIETOR_BIT (1 << 2)

/**
 * @brief The event group associated with user interaction with robot dog
 * 
 * Call `xEventGroupSetBits(interactInputsEG, XXX_BIT)` to clear the event bit
 * Call `xEventGroupClearBits(interactInputsEG, XXX_BIT)` to clear the event bit
 */
extern EventGroupHandle_t interactInputsEG;
int  createInteractInputsEG();
void deleteInteractInputsEG();

/**
 * @brief init function and ISR for capacitance touch sensor
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
 * @brief init function and ISR for limit switch
 *
 * @param pin The pin to be attached to the limit switch
 * @param triggerMode How interrupt is triggered, either RISING or FALLING
 */
void IRAM_ATTR onLimitSwitch();
int initLimitSwitch(const uint8_t& pin, const int& triggerMode);

/**
 * @brief init function and periodic handler for photo resistor
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


#endif // INTERACT_INPUTS_H
