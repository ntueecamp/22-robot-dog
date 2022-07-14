#include "RobotDog.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

EventGroupHandle_t dogEventGroup = NULL;

EventGroupHandle_t createDogEG()
{
    if (dogEventGroup == NULL)
        dogEventGroup = xEventGroupCreate();

    if (dogEventGroup != NULL)
        return dogEventGroup;
    else    // create failed
        return NULL;
}

void deleteDogEG()
{
    vEventGroupDelete(dogEventGroup);
    dogEventGroup = NULL;
}
