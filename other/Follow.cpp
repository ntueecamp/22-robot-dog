#include "Follow.h"

#include "Trace.h"

void handleFollow(void* argv)
{
    Trace dog;
    dog.init();
    vTaskDelay(RADARDELAYTIME / portTICK_PERIOD_MS);

    while (true)
    {
        dog.Move();
        taskYIELD();
    }
}

TaskHandle_t initFollow()
{
    BaseType_t xResult;
    TaskHandle_t followTaskHandle;
    xResult = xTaskCreate(handleFollow,
                           "FollowHandler",
                           2048,     // stack size in words (4 bytes on ESP32), TBD
                           NULL,
                           2,       // priority, >= 2 is good, TBD
                           &followTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    return followTaskHandle;
}
