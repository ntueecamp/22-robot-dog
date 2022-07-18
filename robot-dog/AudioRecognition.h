#ifndef AUDIO_RECOGNITION_H
#define AUDIO_RECOGNITION_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define SAMPLE_RATE   16000
#define TRANS_BUF_LEN 16000

extern int16_t* audioInputBuffer;

typedef struct
{
    uint8_t pin;
    adc1_channel_t channel;
    TaskHandle_t callingTask;
} audio_config_t;
/**
 * @brief Init function and handler of audio recognition
 *
 * @param pin The pin to be attached to the microphone
 *            Available pins: 32, 33, 34, 35, 36, 37, 38, 39
 * @param channel The ADC channel to be used to sample the audio
 */
void handleAudioRecognition(void* argv);
TaskHandle_t initAudioRecognition(const uint8_t& _pin, const adc1_channel_t& _channel);

#endif // AUDIO_RECOGNITION_H
