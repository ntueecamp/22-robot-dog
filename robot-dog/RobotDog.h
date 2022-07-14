#ifndef ROBOT_DOG_H
#define ROBOT_DOG_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

////////////////////////////////
// Event group and event bits //
////////////////////////////////

// Event Bits
#define CAP_TOUCH_BIT      (1 << 0)
#define LIMIT_SWITCH_BIT   (1 << 1)
#define PHOTO_RESISTOR_BIT (1 << 2)
#define FOLLOWING_BIT      (1 << 3)
#define FOLLOW_STOP_BIT    (1 << 4)

/**
 * @brief The event group of the robot dog
 *
 * Call `xEventGroupSetBits(dogEventGroup, XXX_BIT)` to set the event bit
 * Call `xEventGroupClearBits(dogEventGroup, XXX_BIT)` to clear the event bit
 */
extern EventGroupHandle_t dogEventGroup;
EventGroupHandle_t createDogEG();
void deleteDogEG();

#endif // ROBOT_DOG_H
