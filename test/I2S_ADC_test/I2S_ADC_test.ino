#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/adc.h"

// I2S_NUM_0
#define MIC_PIN     34
#define ADC_CHANNEL ADC1_GPIO34_CHANNEL

#define TRANS_BUF_LEN 16000

#define I2S_SAMPLE_RATE 16000
#define I2S_READ_LEN    1024    // no more than 1024

uint8_t micBuffer[2 * I2S_READ_LEN];    // need 16-bit for each sample, but use uint8_t for easier indexing
uint8_t transBuffer[TRANS_BUF_LEN];     // buffer to be shared

void reader(void* argv)
{
  // Serial.println("Start recording");
  // The 4 high bits are the channel
  // real value = ( ((uint16_t)(buff[i + 1] & 0xf) << 8) | (buff[i + 0]) )

  uint8_t* buffer = (uint8_t*)micBuffer;
  int readCount = 0;
  size_t bytes_read;

  // while(true)
  while(readCount < TRANS_BUF_LEN)
  {
    i2s_adc_enable(I2S_NUM_0);
    i2s_read(I2S_NUM_0, micBuffer, sizeof(micBuffer), &bytes_read, portMAX_DELAY);
    i2s_adc_disable(I2S_NUM_0);

    // Serial.println("======");
    for(int i = 0; i < bytes_read; i += 2)
    {
      if(readCount < TRANS_BUF_LEN)
        transBuffer[readCount++] = (buffer1[i + 1] & 0xF) << 4) | (buffer1[i] >> 4);   // rearrange bits and convert to 8-bit
      // Serial.printf("%03x ", ((uint16_t)(buffer1[i + 1] & 0xF) << 8) | buffer1[i]);
      // if ((i + 2) % 64 == 0)
      //  Serial.println();
    }
  }

  // vTaskDelay(5000 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = I2S_READ_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  //install and start i2s driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  //init ADC pad
  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4);

  xTaskCreatePinnedToCore(reader, "ADC_reader", 2048, micBuffer, 1, NULL, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
