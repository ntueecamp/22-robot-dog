#ifndef PULSEIN_H
#define PULSEIN_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct pulseIn_t
{
    uint8_t pin;
    uint8_t state;
    int64_t startTime;
    int64_t endTime;
    TaskHandle_t callingTask;
} PulseIn_t;
/**
 * @brief Modify pulseIn to be thread safe by using interrupts
 *        Should only be used in a Task!!
 *
 * @param pin The pin to be attached to pulse input
 * @param state The state pulseIn should measure, HIGH or LOW
 * @param timeout Timeout before the function returns 0, in us
 *
 */
int pulseInThreadSafe(const uint8_t& pin, const uint8_t& state, const uint32_t timeout = 1000000UL);

#endif // PULSEIN_H
