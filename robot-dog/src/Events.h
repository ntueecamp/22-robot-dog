#ifndef EVENTS_H
#define EVENTS_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define CAP_TOUCH_BIT      (1 << 0)
#define LIMIT_SWITCH_BIT   (1 << 1)
#define PHOTO_RESISTOR_BIT (1 << 2)
#define FOLLOWING_BIT      (1 << 3)
#define FOLLOW_STOP_BIT    (1 << 4)
#define WOOF_BIT           (1 << 5)
#define PARROT_BIT         (1 << 6)

/**
 * @brief The event group of the robot dog
 *
 * Call `xEventGroupSetBits(dogEventGroup, XXX_BIT)` to set the event bit
 * Call `xEventGroupClearBits(dogEventGroup, XXX_BIT)` to clear the event bit
 */
extern EventGroupHandle_t dogEventGroup;
EventGroupHandle_t createDogEG();
void deleteDogEG();

#endif // EVENTS_H
