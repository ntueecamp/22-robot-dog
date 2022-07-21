#include "AudioRecognition.h"

#include "src/Events.h"
#include "src/Microphone.h"
#ifdef AUDIO_RECOGNITION
#include "src/voice-recognition/CommandDetector.h"
#include "Leg.h"
#endif
#ifdef AUDIO_PARROT
#include "Speaker.h"

const uint8_t wavHeader[44] = {
    0x52, 0x49, 0x46, 0x46, 0xA4, 0x3E, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
    0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3E, 0x00, 0x00, 0x80, 0x3E, 0x00, 0x00,
    0x01, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x80, 0x3E, 0x00, 0x00};    // length 44
#endif


void handleAudioRecognition(void* argv)
{
    audio_config_t* audio_config = (audio_config_t*)argv;

#ifdef AUDIO_RECOGNITION
    int16_t* audioInputBuffer = (int16_t*)malloc(sizeof(int16_t) * TRANS_BUF_LEN);

    Microphone mic(audio_config->pin, audio_config->channel, SAMPLE_RATE);
    CommandDetector cmdDetector(audioInputBuffer);
    CmdDectResult result = {CMD_NONSENSE, -1000};    // _nonsense with score -1000
#endif
#ifdef AUDIO_PARROT
    int16_t* audioInputBuffer = (int16_t*)malloc(sizeof(int16_t) * TRANS_BUF_LEN + 44);
    memcpy(audioInputBuffer, wavHeader, 44);

    XT_Wav_Class* parrotSound = NULL;
#endif

    xTaskNotifyGiveIndexed(audio_config->callingTask, 0);

    mic.init();
#ifdef AUDIO_RECOGNITION
    mic.recordAudio(audioInputBuffer, TRANS_BUF_LEN);
#endif
#ifdef AUDIO_PARROT
    mic.recordAudio(audioInputBuffer + 44, TRANS_BUF_LEN);
#endif

    EventBits_t curBits;
    int64_t entryTime = esp_timer_get_time();

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
                {
                    if (esp_timer_get_time() - entryTime > 1000000)     // 1 sec
                        break;
                    else
                        xEventGroupClearBits(dogEventGroup, PHOTO_RESISTOR_BIT);
                }

                vTaskDelay(500 / portTICK_PERIOD_MS);
                mic.recordAudio(audioInputBuffer, TRANS_BUF_LEN);

            #ifdef AUDIO_RECOGNITION
                result = cmdDetector.run();
            
            #ifdef DEBUG
                Serial.printf("detected: %s, with: %.3f\n", commands[result.index], result.score);
            #endif

                // broadcast event
                if (!moving)
                {
                    if (false)
                    {   // go
                        moving = 1;
                    }
                }
                else
                {
                    if (false)          
                    {   // sit
                        dogLeg.write(0.0, 0.0);
                        moving = 0;
                    }
                    else if (result.index == CMD_FORWARD)
                    {   // forward
                        dogLeg.write(0.6, 0.6);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (result.index == CMD_BACKWARD)
                    {   // backward
                        dogLeg.write(-0.6, -0.6);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (result.index == CMD_LEFT)
                    {   // left
                        dogLeg.write(0.4, 0.6);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (result.index == CMD_RIGHT)
                    {   // right
                        dogLeg.write(0.6, 0.4);
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        dogLeg.write(0.0, 0.0);
                    }
                    else if (false)
                    {   // follow
                        break;
                    }
                }
            #endif
            #ifdef AUDIO_PARROT
                if (DacAudio != NULL)
                {
                    parrotSound = new XT_Wav_Class(audioInputBuffer);
                    DacAudio->Play(parrotSound);

                    while (parrotSound->Playing)   // loop until the sound ends
                    {
                        DacAudio->FillBuffer();  // fill buffer to DAC, buffer size is 4000
                        // we need to call this function at least (SAMPLE_RATE / 4000) times per second
                        taskYIELD();
                    }
                    DacAudio->StopAllSounds();

                    delete parrotSound;
                    parrotSound = NULL;
                }
            #endif
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
