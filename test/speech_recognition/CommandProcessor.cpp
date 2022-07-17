#include <Arduino.h>
#include "CommandProcessor.h"

const char *words[] = {
    "follow",
    "stop",
    "go",
    "_nonsense",
};

void commandQueueProcessorTask(void *param)
{
    CommandProcessor *commandProcessor = (CommandProcessor *)param;
    while (true)
    {
        uint16_t commandIndex = 0;
        if (xQueueReceive(commandProcessor->m_command_queue_handle, &commandIndex, portMAX_DELAY) == pdTRUE)
        {
            commandProcessor->processCommand(commandIndex);
        }
    }
}

void CommandProcessor::processCommand(uint16_t commandIndex)
{
    // change this part
    Serial.println(words[commandIndex]);
}

CommandProcessor::CommandProcessor()
{
    // allow up to 5 commands to be in flight at once
    m_command_queue_handle = xQueueCreate(5, sizeof(uint16_t));
    if (!m_command_queue_handle)
    {
        Serial.println("Failed to create command queue");
    }
    // kick off the command processor task
    TaskHandle_t command_queue_task_handle;
    xTaskCreate(commandQueueProcessorTask, "Command Queue Processor", 1024, this, 1, &command_queue_task_handle);
}

void CommandProcessor::queueCommand(uint16_t commandIndex, float best_score)
{
    // unsigned long now = millis();
    // if (commandIndex != 4 && commandIndex != -1)
    // {
    Serial.printf("***** %ld Detected command %s(%f)\n", millis(), words[commandIndex], best_score);
    if (xQueueSendToBack(m_command_queue_handle, &commandIndex, 0) != pdTRUE)
    {
        Serial.println("No more space for command");
    }
    // }
}
