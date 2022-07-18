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
#include "src/esp32-led-matrix/LedMatrix.h"
#include "src/XT_DAC_Audio/XT_DAC_Audio.h"
#include "Events.h"
#include "LEDContent.h"

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

void handleSound(void* argv)
{
    sound_config_t* sound_config = (sound_config_t*)argv;
    XT_DAC_Audio_Class DacAudio(sound_config->pin, 0);
    XT_Wav_Class woofSound(sound_config->soundData);

    xTaskNotifyGiveIndexed(sound_config->callingTask, 0);

    EventBits_t curBits;
    while (true)
    {
        curBits = xEventGroupWaitBits( dogEventGroup,
                                       CAP_TOUCH_BIT | WOOF_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
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
        else if (curBits & WOOF_BIT)
        {
            // set repeat, n -> play n+1 times
            woofSound.Repeat = 1;
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
            // reset repeat
            woofSound.Repeat = 0;

            xEventGroupClearBits(dogEventGroup, WOOF_BIT);
        }
    }

    // should never get here
}

TaskHandle_t initSound(const uint8_t& _pin, const unsigned char* _soundData)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    sound_config_t sound_config = {
        .pin = _pin,
        .soundData = _soundData,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t soundTaskHandle;
    xResult = xTaskCreate( handleSound,
                           "SoundHandler",
                           2048,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)&sound_config,
                           2,       // priority, >= 2 is good, TBD
                           &soundTaskHandle );

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed( 0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return soundTaskHandle;
}

void handleLED(void* argv)
{
    led_config_t* led_config = (led_config_t*)argv;
    LedMatrix ledMatrix(1, led_config->sck, led_config->miso, led_config->mosi, led_config->cs);

    xTaskNotifyGiveIndexed(led_config->callingTask, 0);

    int curDisplay = 0;     // 0 -> pattern, 1 -> text
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

TaskHandle_t initLED(const uint8_t& _sck, const uint8_t& _miso, const uint8_t& _mosi, const uint8_t& _cs)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    led_config_t led_config = {
        .sck  = _sck,
        .miso = _miso,
        .mosi = _mosi,
        .cs   = _cs,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t ledTaskHandle;
    xResult = xTaskCreate( handleLED,
                           "LEDHandler",
                           2048,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)&led_config,
                           2,       // priority, >= 2 is good, TBD
                           &ledTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed( 0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return ledTaskHandle;
}
