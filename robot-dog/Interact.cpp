/**
 * @file Interact.cpp
 * @author ntueecamp 2022 (FrSh28)
 * @brief Inplementations of functions about interaction inputs (CapTouch, LimitSwitch and PhotoResistor)
 *        and outputs (Sound and LED) of the robot dog.
 * @date 2022-06-30
 *
 */

#include "Interact.h"

#include <Arduino.h>
#include "driver/adc.h"
#include "src/Events.h"

void IRAM_ATTR onCapTouchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(dogEventGroup, CAP_TOUCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initCapTouch(const uint8_t& _pin, const uint16_t& _threshold)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return -1;

    touchAttachInterrupt(_pin, onCapTouchISR, _threshold);

    return 0;
}

void IRAM_ATTR onLimitSwitchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(dogEventGroup, LIMIT_SWITCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initLimitSwitch(const uint8_t& _pin, const int& _triggerMode)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return -1;

    if (_triggerMode == RISING)
        pinMode(_pin, INPUT_PULLDOWN);
    else if (_triggerMode == FALLING)
        pinMode(_pin, INPUT_PULLUP);
    else
        return -1;
    
    attachInterrupt(_pin, onLimitSwitchISR, _triggerMode);

    return 0;
}

void handlePhotoResistor(void* argv)
{
    photo_R_config_t* photo_R_config = (photo_R_config_t*)argv;
    uint8_t  pin       = photo_R_config->pin;
    uint16_t threshold = photo_R_config->threshold;
    uint32_t period    = photo_R_config->period/ portTICK_PERIOD_MS;;

    xTaskNotifyGiveIndexed(photo_R_config->callingTask, 0);

    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true)
    {
    #ifdef DEBUG
        int val = analogRead(pin);
        Serial.println(val);
        if (val < threshold)
    #else
        if (analogRead(pin) < threshold)
    #endif
            xEventGroupSetBits(dogEventGroup, PHOTO_RESISTOR_BIT);

        vTaskDelayUntil(&lastWakeTime, period);
    }

    // should never get here
}

TaskHandle_t initPhotoResistor(const uint8_t& _pin, const uint16_t& _threshold, const uint32_t& _period)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    photo_R_config_t photo_R_config = {
        .pin = _pin,
        .threshold = _threshold,
        .period = _period,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t photoResistorTaskHandle;
    xResult = xTaskCreate( handlePhotoResistor,
                           "PhotoResistorHandler",
                           2048,     // stack size in words (4 bytes on ESP32)
                           (void*)&photo_R_config,
                           2,       // priority, >= 2 is good, TBD
                           &photoResistorTaskHandle );

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed( 0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return photoResistorTaskHandle;
}
