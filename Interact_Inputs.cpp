/**
 * @file Interact_Inputs.cpp
 * @author ntueecamp 2022 (FrSh28)
 * @brief Inplementations of functions about interactive inputs (CapTouch, LimitSwitch and PhotoResistor) of the robot dog.
 * @date 2022-06-28
 *
 */

#include "Interact_Inputs.h"

EventGroupHandle_t interactInputsEG;

EventGroupHandle_t createInteractInputsEG()
{
    if (interactInputsEG == NULL)
        interactInputsEG = xEventGroupCreate();

    if (interactInputsEG != NULL)
        return interactInputsEG;
    else    // create failed
        return NULL;
}

void deleteInteractInputsEG()
{
    vEventGroupDelete(interactInputsEG);
    interactInputsEG = NULL;
}

void IRAM_ATTR onCapTouchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(interactInputsEG, CAP_TOUCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initCapTouch(const uint8_t& pin, const uint16_t& threshold)
{
    if (interactInputsEG == NULL)
        if (createInteractInputsEG() == NULL)   // create failed
            return -1;

    touchAttachInterrupt(pin, onCapTouchISR, threshold);

    return 0;
}

void IRAM_ATTR onLimitSwitchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(interactInputsEG, LIMIT_SWITCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initLimitSwitch(const uint8_t& pin, const int& triggerMode)
{
    if (interactInputsEG == NULL)
        if (createInteractInputsEG() == NULL)   // create failed
            return -1;

    if (triggerMode == RISING)
        pinMode(pin, INPUT_PULLDOWN);
    else if (triggerMode == FALLING)
        pinMode(pin, INPUT_PULLUP);
    else
        return -1;
    
    attachInterrupt(pin, onLimitSwitchISR, triggerMode);

    return 0;
}

void handlePhotoResistor(void* argv)
{
    uint8_t  pin       =  (uint8_t)((uint32_t*)argv)[0];
    uint16_t threshold = (uint16_t)((uint32_t*)argv)[1];
    uint32_t period    =           ((uint32_t*)argv)[2];
    // free(argv);

    period = period / portTICK_PERIOD_MS;
    TickType_t lastWakeTime = xTaskGetTickCount();

    while (true)
    {
        lastWakeTime = xTaskGetTickCount();
        if (analogRead(pin) < threshold)
            xEventGroupSetBits(interactInputsEG, PHOTO_RESIETOR_BIT);
        
        vTaskDelayUntil(&lastWakeTime, period);
    }

    // should never get here
}

TaskHandle_t initPhotoResistor(const uint8_t& pin, const uint16_t& threshold, const uint32_t& period)
{
    if (interactInputsEG == NULL)
        if (createInteractInputsEG() == NULL)   // create failed
            return NULL;

    //uint32_t* argv = (uint32_t*)malloc(2 * sizeof(uint32_t));
    uint32_t argv[3];
    argv[0] = pin;
    argv[1] = threshold;
    argv[2] = period;

    BaseType_t xResult;
    TaskHandle_t photoResistorTaskHandle;
    xResult = xTaskCreate( handlePhotoResistor,
                           "HandlePhotoResistor",
                           1024,     // stack size in words (4 bytes on ESP32)
                           (void*)argv,
                           2,       // priority, >= 2 is good, TBD
                           &photoResistorTaskHandle );

    if (xResult != pdPASS)
        return NULL;

    return photoResistorTaskHandle;
}
