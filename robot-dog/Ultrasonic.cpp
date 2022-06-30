#include "Ultrasonic.h"

#include "esp_timer.h"

#define RISING_BIT  (1 << 0)
#define FALLING_BIT (1 << 1)

typedef struct pulseIn_t
{
    int64_t startTime;
    int64_t endTime;
    uint8_t state;
    EventGroupHandle_t pulseInEG;
} PulseIn_t;

void IRAM_ATTR onRising(void* _curPulseIn)
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    PulseIn_t* curPulseIn = (PulseIn_t*)_curPulseIn;

    if (curPulseIn->state == HIGH)
    {
        curPulseIn->startTime = esp_timer_get_time();
    }
    else if (curPulseIn->state == LOW)
    {
        curPulseIn->endTime = esp_timer_get_time();
        if (FALLING_BIT & xEventGroupGetBitsFromISR(curPulseIn->pulseInEG) == 0)     // hasn't detected falling edge
        {
            curPulseIn->endTime = -1;
            return;
        }
    }

    xResult = xEventGroupSetBitsFromISR(curPulseIn->pulseInEG, RISING_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

void IRAM_ATTR onFalling(void* _curPulseIn)
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    PulseIn_t* curPulseIn = (PulseIn_t*)_curPulseIn;

    if (curPulseIn->state == LOW)
    {
        curPulseIn->startTime = esp_timer_get_time();
    }
    else if (curPulseIn->state == HIGH)
    {
        curPulseIn->endTime = esp_timer_get_time();
        if (RISING_BIT & xEventGroupGetBitsFromISR(curPulseIn->pulseInEG) == 0)     // hasn't detected rising edge
        {
            curPulseIn->endTime = -1;
            return;
        }
    }

    xResult = xEventGroupSetBitsFromISR(curPulseIn->pulseInEG, FALLING_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int pulseInThreadSafe(const uint8_t& pin, const uint8_t& state, const uint32_t timeout)
{
    bool success = true;
    PulseIn_t curPulseIn;
    curPulseIn.startTime = -1;
    curPulseIn.endTime   = -1;       // indicate the interrupt getting endtime cannot set event flag before the other
    curPulseIn.state     = state;
    curPulseIn.pulseInEG = xEventGroupCreate();

    if (state == HIGH)
    {
        attachInterruptArg(pin,  onRising, (void*)&curPulseIn,  RISING);
        attachInterruptArg(pin, onFalling, (void*)&curPulseIn, FALLING);
    }
    else if (state == LOW)
    {
        attachInterruptArg(pin, onFalling, (void*)&curPulseIn, FALLING);
        attachInterruptArg(pin,  onRising, (void*)&curPulseIn,  RISING);
    }

    EventBits_t curBits;
    curBits = xEventGroupWaitBits( curPulseIn.pulseInEG,
                                   RISING_BIT | FALLING_BIT,
                                   pdTRUE,
                                   pdTRUE,
                                   (timeout / 1000) / portTICK_PERIOD_MS );
    if ( curBits & (RISING_BIT | FALLING_BIT) != (RISING_BIT | FALLING_BIT)   // timeout
          || curPulseIn.startTime < 0 || curPulseIn.endTime < 0)
        success = false;

    detachInterrupt(pin);
    vEventGroupDelete(curPulseIn.pulseInEG);

    if (success)
        return curPulseIn.endTime - curPulseIn.startTime;
    else
        return 0;
}
