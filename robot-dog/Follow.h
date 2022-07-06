#ifndef FOLLOW_H
#define FOLLOW_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Trace.h"

/**
 * @brief Init function and handler for following
 * 
 */
void handleFollow(void* argv);
TaskHandle_t initFollow();

#endif // FOLLOW_H
