#include "Speaker.h"

#include "src/XT_DAC_Audio/XT_DAC_Audio.h"
#include "src/Events.h"
#include "woof.h"

XT_DAC_Audio_Class* DacAudio;

void handleSound(void* argv)
{
    sound_config_t* sound_config = (sound_config_t*)argv;
    if (DacAudio == NULL)
        DacAudio = new XT_DAC_Audio_Class(sound_config->pin, 0);
    XT_Wav_Class woofSound(sound_config->soundData);

    xTaskNotifyGiveIndexed(sound_config->callingTask, 0);

    EventBits_t curBits;
    while (true)
    {
        curBits = xEventGroupWaitBits(dogEventGroup,
                                       CAP_TOUCH_BIT | WOOF_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
                                       pdFALSE,   // true -> clear the bits before returning, won't affect returned value
                                       pdFALSE,   // true -> wait for all
                                       portMAX_DELAY);

        if (curBits & CAP_TOUCH_BIT)
        {
        #ifdef DEBUG
            Serial.println("CAP");
        #endif

            // start sound playing
            DacAudio->Play(&woofSound);  // register to play

            while (woofSound.Playing)   // loop until the sound ends
            {
                DacAudio->FillBuffer();  // fill buffer to DAC, buffer size is 4000
                // we need to call this function at least (SAMPLE_RATE / 4000) times per second
                taskYIELD();
            }
            DacAudio->StopAllSounds();
            // end sound playing

            xEventGroupClearBits(dogEventGroup, CAP_TOUCH_BIT);
        }
        else if (curBits & WOOF_BIT)
        {
            // set repeat, n -> play n+1 times
            woofSound.Repeat = 1;
            // start sound playing
            DacAudio->Play(&woofSound);  // register to play

            while (woofSound.Playing)   // loop until the sound ends
            {
                DacAudio->FillBuffer();  // fill buffer to DAC, buffer size is 4000
                // we need to call this function at least (SAMPLE_RATE / 4000) times per second
                taskYIELD();
            }
            DacAudio->StopAllSounds();
            // end sound playing
            // reset repeat
            woofSound.Repeat = 0;

            xEventGroupClearBits(dogEventGroup, WOOF_BIT);
        }
    }

    // should never get here
}

TaskHandle_t initSound(const uint8_t& _pin)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    sound_config_t sound_config = {
        .pin = _pin,
        .soundData = woof,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t soundTaskHandle;
    xResult = xTaskCreate(handleSound,
                           "SoundHandler",
                           2048,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)&sound_config,
                           2,       // priority, >= 2 is good, TBD
                           &soundTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed(0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return soundTaskHandle;
}
