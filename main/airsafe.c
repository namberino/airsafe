#include <stdbool.h>
#include <stdio.h>
#include <esp_log.h>
#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "HD44780.h"
#include "portmacro.h"
#include "sdkconfig.h"

#define LCD_ADDR 0x27
#define SDA_PIN 21
#define SCL_PIN 22
#define LCD_COLS 16
#define LCD_ROWS 2

void lcd_task(void* param)
{
	char num[20];

	while (true)
	{
		LCD_home();
		LCD_clearScreen();
		LCD_writeStr("LCD test");
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		LCD_clearScreen();
		LCD_writeStr("Count from 1-10:");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		LCD_clearScreen();

		for (int i = 0; i <= 10; i++)
		{
			LCD_setCursor(8, 2);
			sprintf(num, "%d", i);
			LCD_writeStr(num);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
	}
}

void app_main(void)
{
	LCD_init(LCD_ADDR, SDA_PIN, SCL_PIN, LCD_COLS, LCD_ROWS);
	xTaskCreate(&lcd_task, "LCD test", 2048, NULL, 5, NULL);
}
