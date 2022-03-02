#include <Arduino.h>
#include "shared.cpp"

#define LED_PROBLEM 16
#define LED_HEARTBEAT 17

void TaskBlink(void *pvParameters)
{
    (void)pvParameters;

    pinMode(LED_PROBLEM, OUTPUT);
    pinMode(LED_HEARTBEAT, OUTPUT);

    for (;;)
    {
        if (state == STATE_AP)
        {
            for (int i = 0; i < 2; i++)
            {
                digitalWrite(LED_PROBLEM, HIGH);
                vTaskDelay(250);
                digitalWrite(LED_PROBLEM, LOW);
                vTaskDelay(250);
            }
            vTaskDelay(2000);
        }
        else if (state == STATE_STA_CONNECTED_SERVER_DOWN)
        {
            digitalWrite(LED_PROBLEM, HIGH);
            vTaskDelay(250);
            digitalWrite(LED_PROBLEM, LOW);
            vTaskDelay(2750);
        }
        else if (state == STATE_STA_CONNECTED_SERVER_UP)
        {
            digitalWrite(LED_HEARTBEAT, HIGH);
            vTaskDelay(250);
            digitalWrite(LED_HEARTBEAT, LOW);
            vTaskDelay(750);
        }
    }
}
