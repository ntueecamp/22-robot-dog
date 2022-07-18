#include "AudioRecognition.h"

#include "Events.h"
#include "Microphone.h"

uint8_t audioInputBuffer[TRANS_BUF_LEN];

void handleAudioRecognition(void* argv)
{
    audio_config_t* audio_config = (audio_config_t*)argv;

    Microphone mic(audio_config->pin, audio_config->channel, SAMPLE_RATE);
    // NeuralNetwork
    xTaskNotifyGiveIndexed(audio_config->callingTask, 0);

    mic.init();

    EventBits_t curBits;

    while (true)
    {
        curBits = xEventGroupWaitBits( dogEventGroup,
                                       PHOTO_RESISTOR_BIT | FOLLOW_STOP_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
                                       pdFALSE,   // true -> clear the bits before returning, won't affect returned value
                                       pdTRUE,    // true -> wait for all
                                       portMAX_DELAY);

        if ((curBits & PHOTO_RESISTOR_BIT) && (curBits & FOLLOW_STOP_BIT))
        {
#ifdef DEBUG
            Serial.println("PHR");
#endif // DEBUG

            // sample audio
            mic.recordAudio(audioInputBuffer, TRANS_BUF_LEN);

            // send to NN
            // predict
            // broadcast event
            vTaskDelay(2000 / portTICK_PERIOD_MS);

            xEventGroupClearBits(dogEventGroup, PHOTO_RESISTOR_BIT | FOLLOW_STOP_BIT);
            xEventGroupSetBits(dogEventGroup, FOLLOWING_BIT);
        }
    }

    // should never get here
}

TaskHandle_t initAudioRecognition(const uint8_t& _pin, const adc1_channel_t& _channel)
{
    if (dogEventGroup == NULL && createDogEG() == NULL)   // create failed
        return NULL;

    audio_config_t audio_config = {
        .pin = _pin,
        .channel = _channel,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t audioRecognitionTaskHandle;
    xResult = xTaskCreate( handleAudioRecognition,
                           "AudioRecognitionHandler",
                           2048,     // stack size in words (4 bytes on ESP32)
                           (void*)&audio_config,
                           2,       // priority, >= 2 is good, TBD
                           &audioRecognitionTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed( 0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return audioRecognitionTaskHandle;
}
