#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define SAMPLE_RATE   16000
#define TRANS_BUF_LEN 16000

extern uint8_t audioInputBuffer[TRANS_BUF_LEN];

/**
 * @brief Init function and handler of audio input from microphone
 *        Microphone ustilizes esp32's built-in I2S_ADC on I2S0
 * 
 * @param pin The pin to be attached to the microphone
 *            Available pins: 32, 33, 34, 35, 36, 37, 38, 39
 */
void handleMicrophone(void* argv);
TaskHandle_t initMicrophone(const uint8_t& pin, const adc1_channel_t& channel, const TaskHandle_t& audioProcessorHandle = NULL);

#endif // MICROPHONE_H
