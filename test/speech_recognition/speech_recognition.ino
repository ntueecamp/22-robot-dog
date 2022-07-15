#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/adc.h"
#include "CommandDetector.h"
#include "CommandProcessor.h"
#include "ADCSampler.h"

#define DEBUG

#define LED_PIN 2

// I2S_NUM_0
#define MIC_PIN 34
#define ADC_CHANNEL ADC1_CHANNEL_6 // GPIO 34

#define TRANS_BUF_LEN 16000

#define I2S_SAMPLE_RATE 16000
#define I2S_BUF_LEN 1024 // no more than 1024

uint8_t micBuffer[2 * I2S_BUF_LEN];      // need 16-bit for each sample, but use uint8_t for easier indexing
uint8_t audioInputBuffer[TRANS_BUF_LEN]; // buffer to be shared

// This task does all the heavy lifting for our application
void commandDetectTask(void *param)
{
  CommandDetector *commandDetector = static_cast<CommandDetector *>(param);

  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);
  while (true)
  {
    // wait for some audio samples to arrive
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
    if (ulNotificationValue > 0)
    {
      commandDetector->run();
    }
  }
}

void setup()
{
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
      .fixed_mclk = 0};
  // // install and start i2s driver
  // i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  // // init ADC pad
  // i2s_set_adc_mode(ADC_UNIT_1, ADC_CHANNEL);

  // start up the I2S input (from either an I2S microphone or Analogue microphone via the ADC)
  I2SSampler *i2s_sampler = new ADCSampler(ADC_UNIT_1, ADC1_CHANNEL_6);
  // the command processor
  CommandProcessor *command_processor = new CommandProcessor();

  // create our application
  CommandDetector *commandDetector = new CommandDetector(i2s_sampler, command_processor);

  // set up the i2s sample writer task
  TaskHandle_t applicationTaskHandle;
  xTaskCreatePinnedToCore(commandDetectTask, "Command_Detect", 8192, commandDetector, 1, &applicationTaskHandle, 0);

  // start sampling from i2s device - use I2S_NUM_0 as that's the one that supports the internal ADC
  i2s_sampler->start(I2S_NUM_0, i2s_config, applicationTaskHandle);

  // xTaskCreatePinnedToCore(reader, "ADC_reader", 2500, micBuffer, 1, NULL, 1);
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}
