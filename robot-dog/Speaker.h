#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


class XT_DAC_Audio_Class;
extern XT_DAC_Audio_Class* DacAudio;

typedef struct
{
    uint8_t pin;
    const unsigned char* soundData;
    TaskHandle_t callingTask;
} sound_config_t;

/**
 * @brief Init function and infinite running handler related to sound
 *
 * @param pin The pin to output the sound (by DAC), GPIO 25 or 26
 */
void handleSound(void* argv);
TaskHandle_t initSound(const uint8_t& _pin);


#endif // SPEAKER_H
