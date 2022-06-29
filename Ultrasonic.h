#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Modify pulseIn to be thread safe by using interrupts
 *        Should only be used in a Task!!
 *
 * @param pin The pin to be attached to pulse input
 * @param state The state pulseIn should measure, HIGH or LOW
 * @param timeout Timeout before the function returns 0
 *
 */
int pulseInThreadSafe(const uint8_t& pin, const uint8_t& state, const uint32_t timeout = 1000000UL);

#endif // ULTRASONIC_H
