#include "Microphone.h"

#include <algorithm>

Microphone::Microphone(const uint8_t& _pin, const adc1_channel_t& _channel, const uint32_t& _sampleRate)
 : pin(_pin), channel(_channel), sampleRate(_sampleRate)
{
    i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = sampleRate,
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

    micBuffer = (uint8_t*)malloc(2 * I2S_BUFF_LEN);    // need 16-bit (2 bytes) for each sample, but use uint8_t for easier indexing
}

Microphone::~Microphone()
{
    i2s_driver_uninstall(I2S_NUM_0);
    free(micBuffer);
}

void Microphone::init()
{
    // // checking if input arguments are legal
    // uint8_t channel2Pin;
    // if (adc1_pad_get_io_num(channel, &channel2Pin) != ESP_OK)
    //     return;
    // if (pin != channel2Pin)
    //     return;

    // install and start i2s driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    //init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, channel);
}

int Microphone::recordAudio(int16_t* buf, const int& length)
{
    uint32_t index = 0, total = 0;
    uint16_t minVal = 65535, maxVal = 0;
    size_t samplesAcquired = 0, bytesRead = 0;
    
    while (index < length)
    {
        samplesAcquired = length - index;
        if (samplesAcquired > I2S_BUFF_LEN)
            samplesAcquired = I2S_BUFF_LEN;

        i2s_adc_enable(I2S_NUM_0);
        i2s_read(I2S_NUM_0, micBuffer, samplesAcquired * 2, &bytesRead, portMAX_DELAY);
        i2s_adc_disable(I2S_NUM_0);

        for (int i = 0; i < bytesRead; i += 2)
        {
            buf[index] = (((int16_t)micBuffer[i + 1] & 0xF) << 8) | micBuffer[i];   // rearrange bits and convert to 8-bit

#ifdef DEBUG
            Serial.printf("%03x ", (((uint16_t)micBuffer[i + 1] & 0xF) << 8) | micBuffer[i]);
            if ((i + 2) % 64 == 0)
                Serial.println();
#endif

            total += buf[index];
            if (buf[index] < minVal)
                minVal = buf[index];
            if (buf[index] > maxVal)
                maxVal = buf[index];
            index++;
        }
    }

    float mean = (float)total / length;
    float scale = 20000.0 / (float)std::max(mean - minVal, maxVal - mean);    // 2^15 = 32768
    for (int i = 0; i < length; i++)
        buf[i] = (buf[i] - mean) * scale;

    return index;
}
