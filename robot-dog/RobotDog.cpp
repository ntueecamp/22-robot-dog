#include "RobotDog.h"


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
