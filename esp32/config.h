#ifndef CONFIG_H
#define CONFIG_H

#define ARDUINOTRACE_ENABLE 0
#include <ArduinoTrace.h>

#define DUMMY_SPEED_DATA 0
#define LVGL_MEMORY_PROF 0

#define SCHEDULER_SOURCE millis()

#define PIN_MISO      5
#define PIN_MOSI      6
#define PIN_SCLK      7
#define PIN_LCD_CS    14
#define PIN_LCD_DC    15
#define PIN_LCD_RST   21
#define PIN_BACKLIGHT 22

/* from user_config.h */
// I2C
#define ESP32_SCL_NUM (GPIO_NUM_8)
#define ESP32_SDA_NUM (GPIO_NUM_18)


#define Backlight_Testing

#define LCD_H_RES 466
#define LCD_V_RES 466
#define LVGL_BUF_HEIGHT 50
#define SCREEN_WIDTH  466
#define SCREEN_HEIGHT 466


#define LCD_CS_PIN         GPIO_NUM_10
#define LCD_PCLK_PIN       GPIO_NUM_11
#define LCD_D0_PIN         GPIO_NUM_4
#define LCD_D1_PIN         GPIO_NUM_5
#define LCD_D2_PIN         GPIO_NUM_6
#define LCD_D3_PIN         GPIO_NUM_7
#define LCD_RST_PIN        GPIO_NUM_3
#define BK_LIGHT_PIN       (-1)

#define DISP_TOUCH_ADDR                   0x38
#define EXAMPLE_PIN_NUM_TOUCH_RST         (-1)
#define EXAMPLE_PIN_NUM_TOUCH_INT         (-1)

#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1
#define LVGL_TASK_STACK_SIZE   (8 * 1024) /*lvgl 9 A larger cache is needed.*/
#define LVGL_TASK_PRIORITY     5

#endif
