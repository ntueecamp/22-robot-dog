#include "Microphone.h"

#include "driver/i2s.h"

#define I2S_BUFF_LEN  1024    // no more than 1024

uint8_t audioInputBuffer[TRANS_BUF_LEN];

void handleMicrophone(void* argv)
{
    TaskHandle_t audioProcessorHandle = *((TaskHandle_t*)argv);
    uint8_t micBuffer[2 * I2S_BUFF_LEN];    // need 16-bit for each sample, but use uint8_t for easier indexing
    size_t samplesAcquired, bytesRead;      // each sample has two bytes (16-bit)
    int index = 0;

    while (true)
    {
        if (audioProcessorHandle != NULL)
            ulTaskNotifyTakeIndexed( 0,         // take from the 0th notification
                                     pdTRUE,    // true -> clear the value before returning, won't affect returned value
                                     portMAX_DELAY);
        
        while (index < TRANS_BUF_LEN)
        {
            samplesAcquired = TRANS_BUF_LEN - index;
            if (samplesAcquired > I2S_BUFF_LEN)
                samplesAcquired = I2S_BUFF_LEN;

            i2s_adc_enable(I2S_NUM_0);
            i2s_read(I2S_NUM_0, micBuffer, samplesAcquired * 2, &bytesRead, portMAX_DELAY);
            i2s_adc_disable(I2S_NUM_0);

            for (int i = 0; i < bytesRead; i += 2)
            {
                audioInputBuffer[index++] = ((micBuffer[i + 1] & 0xF) << 4) | (micBuffer[i] >> 4);   // rearrange bits and convert to 8-bit
            #ifdef DEBUG
                Serial.printf("%03x ", (((micBuffer[i + 1] & 0xF) << 4) | (micBuffer[i] >> 4)) );
                if ((i + 2) % 64 == 0)
                    Serial.println();
            #endif // DEBUG
            }
        }

        if (audioProcessorHandle != NULL)
            xTaskNotifyGiveIndexed(audioProcessorHandle, 0);
    }

    // should never get here
}

TaskHandle_t initMicrophone(const uint8_t& pin, const adc1_channel_t& channel, const TaskHandle_t& audioProcessorHandle)
{
    // // checking if input arguments are legal
    // uint8_t channel2Pin;
    // if (adc1_pad_get_io_num(channel, &channel2Pin) != ESP_OK)
    //     return NULL;
    // if (pin != channel2Pin)
    //     return NULL;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = I2S_BUFF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    //install and start i2s driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    //init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, channel);

    BaseType_t xResult;
    TaskHandle_t microphoneTaskHandle;
    xResult = xTaskCreate( handleMicrophone,
                           "MicrophoneHandler",
                           2048,     // stack size in words (4 bytes on ESP32)
                           (void*)audioProcessorHandle,
                           2,       // priority, >= 2 is good, TBD
                           &microphoneTaskHandle);

    if (xResult != pdPASS)
        return NULL;

    return microphoneTaskHandle;
}
