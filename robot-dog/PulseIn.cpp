#include "PulseIn.h"

#include "esp_timer.h"

#define RISING_BIT  (1 << 0)
#define FALLING_BIT (1 << 1)

typedef struct pulseIn_t
{
    int64_t startTime;
    int64_t endTime;
    uint8_t state;
    TaskHandle_t callingTask;
} PulseIn_t;

void IRAM_ATTR pulseInISR(void* _curPulseIn)
{
    BaseType_t xHigherPriorityWoken = pdFALSE;
    PulseIn_t* curPulseIn = (PulseIn_t*)_curPulseIn;

    if (digitalRead(pin) == HIGH)
    {
        if (curPulseIn->state == HIGH)
        {
            curPulseIn->startTime = esp_timer_get_time();
        }
        else if (curPulseIn->state == LOW && curPulseIn->startTime > 0)
        {
            curPulseIn->endTime = esp_timer_get_time();
            xTaskNotifyIndexedFromISR( curPulseIn->callingTask,     // task to notify
                                       0,                           // set the 0th notification
                                       0x01                         // value to be sent
                                       eSetBits,
                                       &xHigherPriorityWoken);
            portYIELD_FROM_ISR(xHigherPriorityWoken);

            return;
        }
    }
    else    // LOW
    {
        if (curPulseIn->state == LOW)
        {
            curPulseIn->startTime = esp_timer_get_time();
        }
        else if (curPulseIn->state == HIGH && curPulseIn->startTime > 0)
        {
            curPulseIn->endTime = esp_timer_get_time();
            xTaskNotifyIndexedFromISR( curPulseIn->callingTask,     // task to notify
                                       0,                           // set the 0th notification
                                       0x01                         // value to be sent
                                       eSetBits,
                                       &xHigherPriorityWoken);
            portYIELD_FROM_ISR(xHigherPriorityWoken);

            return;
        }
    }
}

int pulseInThreadSafe(const uint8_t& pin, const uint8_t& state, const uint32_t timeout)
{
    bool success = true;
    PulseIn_t curPulseIn;
    curPulseIn.startTime   = -1;
    curPulseIn.endTime     = -1;
    curPulseIn.state       = state;
    curPulseIn.callingTask = xTaskGetCurrentTaskHandle();

    attachInterruptArg(pin, pulseInISR, (void*)&curPulseIn, CHANGE);

    BaseType_t result = pdFALSE;
    result = xTaskNotifyWaitIndexed ( 0,            // wait on 0th notification
                                      0x00,         // bits to clear on entry
                                      ULONG_MAX,    // bits to clear on exit
                                      NULL,         // bits set before exit
                                      (timeout / 1000) / portTICK_PERIOD_MS);

    detachInterrupt(pin);

    if (result == pdFALSE || curPulseIn.startTime < 0 || curPulseIn.endTime < 0)   // timeout
        return 0;
    else
        return curPulseIn.endTime - curPulseIn.startTime;

}
