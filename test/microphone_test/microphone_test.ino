#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/adc.h"

//#define DEBUG

#define LED_PIN 2

// I2S_NUM_0
#define MIC_PIN     34
#define ADC_CHANNEL ADC1_CHANNEL_6   // GPIO 34

#define TRANS_BUF_LEN 16000

#define I2S_SAMPLE_RATE 16000
#define I2S_BUF_LEN    1024    // no more than 1024

uint8_t micBuffer[2 * I2S_BUF_LEN];    // need 16-bit for each sample, but use uint8_t for easier indexing
uint8_t audioInputBuffer[TRANS_BUF_LEN];     // buffer to be shared

void reader(void* argv)
{

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = I2S_BUF_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  //install and start i2s driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  //init ADC pad
  i2s_set_adc_mode(ADC_UNIT_1, ADC_CHANNEL);

  // xTaskCreatePinnedToCore(reader, "ADC_reader", 2500, micBuffer, 1, NULL, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("Start recording");
  // The 4 high bits are the channel
  // real value = ( ((uint16_t)(buff[i + 1] & 0xf) << 8) | (buff[i + 0]) )

  size_t samplesAcquired, bytesRead;      // each sample has two bytes (16-bit)
  int index = 0;

  while (true)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    
    index = 0;
    while (index < TRANS_BUF_LEN)
    {
      samplesAcquired = TRANS_BUF_LEN - index;
      if (samplesAcquired > I2S_BUF_LEN)
        samplesAcquired = I2S_BUF_LEN;
  
      i2s_adc_enable(I2S_NUM_0);
      i2s_read(I2S_NUM_0, micBuffer, samplesAcquired * 2, &bytesRead, portMAX_DELAY);
      i2s_adc_disable(I2S_NUM_0);
  
      for (int i = 0; i < bytesRead; i += 2)
      {
        audioInputBuffer[index++] = ((micBuffer[i + 1] & 0xF) << 4) | (micBuffer[i] >> 4);   // rearrange bits and convert to 8-bit
      #ifdef DEBUG
        Serial.printf("%02x ", (((micBuffer[i + 1] & 0xF) << 4) | (micBuffer[i] >> 4)) );
        if ((i + 2) % 64 == 0)
          Serial.println();
      #endif // DEBUG
      }
    }
  
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
  
}
