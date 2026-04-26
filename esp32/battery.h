#ifndef BATTERY_H
#define BATTERY_H

#include "src/button_bsp/button_bsp.h"
#include "src/tca9554/esp_io_expander_tca9554.h"
#include "ui.h"

/* XXX: rough calc.. */
#define MAX_BATTERY_VOLT 4.2
#define MIN_BATTERY_VOLT 3.2

extern i2c_master_bus_handle_t user_i2c_port0_handle;
esp_io_expander_handle_t io_expander = NULL;

static void button_task(void* parmeter)
{
	uint8_t even_flag = 0x01;
	uint8_t ticks = 0;
	for (;;)
	{
		EventBits_t even = xEventGroupWaitBits(key_groups,
						       BIT_EVEN_ALL, pdTRUE,pdFALSE,
						       pdMS_TO_TICKS(2 * 1000));
		if(READ_BIT(even,12))
		{
			if(READ_BIT(even_flag,1))
			{
				esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);
			}
		}
		if(!READ_BIT(even_flag,1))  //
		{
			ticks++;
			if(READ_BIT(even,10) || (ticks == 4))
			{
				SET_BIT(even_flag,1);
			}
		}
	}
}


void initBatery() {
	pinMode(A0, INPUT);         // Configure A0 as ADC input

	esp_io_expander_new_i2c_tca9554(user_i2c_port0_handle,
                                        ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander);
	esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_6,
                                IO_EXPANDER_OUTPUT);
	esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 1);

	user_button_init();
	xTaskCreatePinnedToCore(button_task,
				"button_task", 4 * 1024, NULL, 2, NULL,0);
}

void readBattery() {
	uint32_t Vbatt = 0;
	if (UI::screen_current != UI::screen_splash)
		return;

	for(int i = 0; i < 16; i++) {
		Vbatt += analogReadMilliVolts(A0); // Read and accumulate ADC voltage
	}
	float Vbattf = 2 * Vbatt / 16 / 1000.0; 
	int battP = ((Vbattf - MIN_BATTERY_VOLT)/(MAX_BATTERY_VOLT-MIN_BATTERY_VOLT)) * 100;
	Data::setBatteryCap(Vbattf, battP);
	Serial.printf("Battery: %3f V/%d(%%)\n", Vbattf, battP);
}

#endif /* BATTERY_H */
