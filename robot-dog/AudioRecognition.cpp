#include "AudioRecognition.h"

#include "src/voice-recognition/CommandDetector.h"
#include "Events.h"
#include "Microphone.h"
#include "Leg.h"

#include <Arduino.h>

int16_t* audioInputBuffer;

void handleAudioRecognition(void* argv)
{
    audio_config_t* audio_config = (audio_config_t*)argv;

    int16_t* audioInputBuffer = (int16_t*)malloc(sizeof(int16_t) * TRANS_BUF_LEN);

    Microphone mic(audio_config->pin, audio_config->channel, SAMPLE_RATE);
    CommandDetector cmdDetector(audioInputBuffer);
    CmdDectResult result = {0, NUMBER_COMMANDS};    // "_nonsense" with score 0
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
            xEventGroupClearBits(dogEventGroup, PHOTO_RESISTOR_BIT);

            int moving = 1;
            while (true)
            {
                curBits = xEventGroupGetBits(dogEventGroup);
                if (curBits & PHOTO_RESISTOR_BIT)
                    break;

                mic.recordAudio(audioInputBuffer, TRANS_BUF_LEN);

                result = cmdDetector.run();
            
            #ifdef DEBUG
                Serial.printf("detected: %s, with: %.3f\n", commands[result.index], result.score);
            #endif
                Serial.printf("detected: %s, with: %.3f\n", commands[result.index], result.score);

                // broadcast event
                if (!moving)
                {
                    if (result.index == 2)     // go
                        moving = 1;
                }
                else
                {
                    if (result.index == 1)          // sit
                    {
                        dogLeg.write(0.0, 0.0);
                        moving = 0;
                    }
                    else if (false)     // forward
                    {
                        dogLeg.write(0.8, 0.8);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (false)     // backward
                    {
                        dogLeg.write(-0.8, -0.8);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (false)     // left
                    {
                        dogLeg.write(0.4, 0.8);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (false)     // right
                    {
                        dogLeg.write(0.8, 0.4);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (false)// (result.index == 0)     // follow
                    {
                        // hardReset();
                        break;
                    }
                }
                taskYIELD();
            }   // inner while

            xEventGroupSetBits(dogEventGroup, WOOF_BIT);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            xEventGroupClearBits(dogEventGroup, FOLLOW_STOP_BIT | PHOTO_RESISTOR_BIT);
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
                           4096,     // stack size in words (4 bytes on ESP32)
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
