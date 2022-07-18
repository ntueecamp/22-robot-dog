#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "driver/adc.h"
#include "driver/i2s.h"

#define I2S_BUFF_LEN 1024   // no more than 1024

class Microphone
{
private:
    i2s_config_t i2s_config;
    uint8_t pin;
    adc1_channel_t channel;
    uint32_t sampleRate;

    uint8_t* micBuffer;

public:
    /**
     * @brief Construct a new Microphone object
     * 
     * @param _pin The pin to be attached to the microphone
     *             Available pins: 32, 33, 34, 35, 36, 37, 38, 39
     * @param _channel The ADC channel to be used to sample the audio
     */
    Microphone(const uint8_t& _pin, const adc1_channel_t& _channel, const uint32_t& _sampleRate);
    ~Microphone();

    void init();
    /**
     * @brief Fill audio samples (8-bit) into buffer
     * 
     * @param buf The buffer to be filled
     * @param length The number of samples acquired to fill
     * @return int The number of samples filled
     */
    int recordAudio(uint8_t* buf, const int& length);
};

#endif // MICROPHONE_H
