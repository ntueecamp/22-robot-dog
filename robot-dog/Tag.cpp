#include "Tag.h"

#include "src/esp32-led-matrix/LedMatrix.h"
#include "src/Events.h"
#include "LEDContent.h"

void handleLED(void* argv)
{
    led_config_t* led_config = (led_config_t*)argv;
    LedMatrix ledMatrix(1, led_config->sck, led_config->miso, led_config->mosi, led_config->cs);

    xTaskNotifyGiveIndexed(led_config->callingTask, 0);

    int curDisplay = 0;     // 0 -> pattern, 1 -> text
    ledMatrix.init();
    ledMatrix.clear();
    ledMatrix.commit();     // clear led display
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
        curBits = xEventGroupWaitBits(dogEventGroup,
                                       LIMIT_SWITCH_BIT,    // CAP_TOUCH_BIT | LIMIT_SWITCH_BIT | PHOTO_RESISTOR_BIT
                                       pdFALSE,   // true -> clear the bits before returning, won't affect returned value
                                       pdFALSE,   // true -> wait for all
                                       portMAX_DELAY);

        if (curBits & LIMIT_SWITCH_BIT)
        {
        #ifdef DEBUG
            Serial.println("LIM");
        #endif

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
                    ledMatrix.adjectTextOrientation();
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
        .sck = _sck,
        .miso = _miso,
        .mosi = _mosi,
        .cs = _cs,
        .callingTask = xTaskGetCurrentTaskHandle()
    };

    BaseType_t xResult;
    TaskHandle_t ledTaskHandle;
    xResult = xTaskCreate(handleLED,
                           "LEDHandler",
                           2048,     // stack size in words (4 bytes on ESP32), TBD
                           (void*)&led_config,
                           2,       // priority, >= 2 is good, TBD
                           &ledTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    ulTaskNotifyTakeIndexed(0,         // take from the 0th notification
                             pdTRUE,    // true -> clear the value before returning, won't affect returned value
                             portMAX_DELAY);

    return ledTaskHandle;
}
