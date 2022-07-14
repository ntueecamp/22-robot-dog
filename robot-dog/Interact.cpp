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
#include "src/esp32-led-matrix/LedMatrix.h"
#include "src/XT_DAC_Audio/XT_DAC_Audio.h"
#include "Events.h"
#include "LEDContent.h"
#include "woof.h"
#include "low_woof.h"

void IRAM_ATTR onCapTouchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(dogEventGroup, CAP_TOUCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initCapTouch(const uint8_t& pin, const uint16_t& threshold)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return -1;

    touchAttachInterrupt(pin, onCapTouchISR, threshold);

    return 0;
}

void IRAM_ATTR onLimitSwitchISR()
{
    BaseType_t xHigherPriorityWoken = pdFALSE, xResult;
    xResult = xEventGroupSetBitsFromISR(dogEventGroup, LIMIT_SWITCH_BIT, &xHigherPriorityWoken);
    if (xResult != pdFAIL)
        portYIELD_FROM_ISR(xHigherPriorityWoken);
}

int initLimitSwitch(const uint8_t& pin, const int& triggerMode)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
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
        if (analogRead(pin) < threshold)
            xEventGroupSetBits(dogEventGroup, PHOTO_RESISTOR_BIT);
        
        vTaskDelayUntil(&lastWakeTime, period);
    }

    // should never get here
}

TaskHandle_t initPhotoResistor(const uint8_t& pin, const uint16_t& threshold, const uint32_t& period)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    //uint32_t* argv = (uint32_t*)malloc(2 * sizeof(uint32_t));
    uint32_t argv[3];
    argv[0] = pin;
    argv[1] = threshold;
    argv[2] = period;

    BaseType_t xResult;
    TaskHandle_t photoResistorTaskHandle;
    xResult = xTaskCreate( handlePhotoResistor,
                           "PhotoResistorHandler",
                           1024,     // stack size in words (4 bytes on ESP32)
                           (void*)argv,
                           2,       // priority, >= 2 is good, TBD
                           &photoResistorTaskHandle );

    if (xResult != pdPASS)
        return NULL;

    return photoResistorTaskHandle;
}

void handleSound(void* argv)
{
    XT_Wav_Class woofSound(low_woof);
    XT_DAC_Audio_Class DacAudio(*((uint8_t*)argv), 0);

    EventBits_t curBits;
    while (true)
    {
        curBits = xEventGroupWaitBits( dogEventGroup,
                                       CAP_TOUCH_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
                                       pdFALSE,   // true -> clear the bits before returning, won't affect returned value
                                       pdFALSE,   // true -> wait for all
                                       portMAX_DELAY);

        if (curBits & CAP_TOUCH_BIT)
        {
        #ifdef DEBUG
            Serial.println("CAP");
        #endif // DEBUG

            // start sound playing
            DacAudio.Play(&woofSound);  // register to play

            while (woofSound.Playing)   // loop until the sound ends
            {
                DacAudio.FillBuffer();  // fill buffer to DAC, buffer size is 4000
                                        // we need to call this function at least (SAMPLE_RATE / 4000) times per second
                taskYIELD();
            }
            DacAudio.StopAllSounds();
            // end sound playing

            xEventGroupClearBits(dogEventGroup, CAP_TOUCH_BIT);
        }
    }

    // should never get here
}

TaskHandle_t initSound(const uint8_t& pin)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    BaseType_t xResult;
    TaskHandle_t soundTaskHandle;
    xResult = xTaskCreate( handleSound,
                           "SoundHandler",
                           4096,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)&pin,
                           2,       // priority, >= 2 is good, TBD
                           &soundTaskHandle );

    if (xResult != pdPASS)
        return NULL;

    return soundTaskHandle;
}

void handleLED(void* argv)
{
    uint8_t  sck = ((uint8_t*)argv)[0];
    uint8_t miso = ((uint8_t*)argv)[1];
    uint8_t mosi = ((uint8_t*)argv)[2];
    uint8_t   cs = ((uint8_t*)argv)[3];

    int curDisplay = 0;     // 0 -> pattern, 1 -> text
    LedMatrix ledMatrix(1, sck, miso, mosi, cs);
    ledMatrix.init();
    ledMatrix.setIntensity(15);   // 0-15

    // display pattern
    ledMatrix.clear();
    for (int i = 0; i < 8; i++)
        ledMatrix.setColumn(i, ledPattern[i]);
    ledMatrix.commit();
    curDisplay = 0;

    EventBits_t curBits;
    while (true)
    {
        curBits = xEventGroupWaitBits( dogEventGroup,
                                       LIMIT_SWITCH_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
                                       pdFALSE,   // true -> clear the bits before returning, won't affect returned value
                                       pdFALSE,   // true -> wait for all
                                       portMAX_DELAY);

        if (curBits & LIMIT_SWITCH_BIT)
        {
        #ifdef DEBUG
            Serial.println("LIM");
        #endif // DEBUG

            if (curDisplay == 0)
            {
                // display text
                ledMatrix.setText(ledText);

                uint32_t period = 100 / portTICK_PERIOD_MS;    // TBD
                TickType_t lastWakeTime = xTaskGetTickCount();
                for (int i = 0; i < 8 * ledText.length() - 1; i++)
                {
                    ledMatrix.clear();
                    ledMatrix.scrollTextLeft();
                    ledMatrix.drawText();
                    ledMatrix.commit();

                    vTaskDelayUntil(&lastWakeTime, period);   // times per second
                }
                curDisplay = 1;
            }
            else if (curDisplay == 1)
            {
                // display pattern
                ledMatrix.clear();
                for (int i = 0; i < 8; i++)
                    ledMatrix.setColumn(i, ledPattern[i]);
                ledMatrix.commit();
                vTaskDelay(500 / portTICK_PERIOD_MS);
                curDisplay = 0;
            }

            xEventGroupClearBits(dogEventGroup, LIMIT_SWITCH_BIT);
        }
    }
}

TaskHandle_t initLED(const uint8_t& sck, const uint8_t& miso, const uint8_t& mosi, const uint8_t& cs)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    uint8_t argv[4];
    argv[0] = sck;
    argv[1] = miso;
    argv[2] = mosi;
    argv[3] = cs;

    BaseType_t xResult;
    TaskHandle_t ledTaskHandle;
    xResult = xTaskCreate( handleLED,
                           "LEDHandler",
                           4096,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)argv,
                           2,       // priority, >= 2 is good, TBD
                           &ledTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    return ledTaskHandle;
}
