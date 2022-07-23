#ifndef TAG_H
#define TAG_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #define LED_TRIGGER CAP_TOUCH_BIT
#define LED_TRIGGER LIMIT_SWITCH_BIT

typedef struct
{
    uint8_t  sck;
    uint8_t miso;
    uint8_t mosi;
    uint8_t   cs;
    TaskHandle_t callingTask;
} led_config_t;
/**
 * @brief Init function and infinite running handler related to LED
 *
 * @param sck  Serial Clock pin
 * @param miso Master In, Slave Out pin
 * @param mosi Master Out, Slave In pin
 * @param cs   Chip Select pin
 */
void handleLED(void* argv);
TaskHandle_t initLED(const uint8_t& _sck, const uint8_t& _miso, const uint8_t& _mosi, const uint8_t& _cs);

#endif // TAG_H
