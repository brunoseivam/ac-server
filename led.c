#include "led.h"
#include "gpio.h"

/*
 * LED Pin: Oak's P1 = ESP's GPIO5
 */
#define LED_IO_MUX   PERIPHS_IO_MUX_GPIO5_U
#define LED_IO_NUM   5
#define LED_IO_FUNC  FUNC_GPIO5
#define LED_IO_MASK  (1 << LED_IO_NUM)

/*#define LED_IO_MUX   PERIPHS_IO_MUX_GPIO4_U
#define LED_IO_NUM   4
#define LED_IO_FUNC  FUNC_GPIO4
#define LED_IO_MASK  (1 << LED_IO_NUM)*/

void led_init (void)
{
    gpio_output_set(0, 0, LED_IO_MASK, 0);
}

void led_toggle (void)
{
    led_set(!led_is_set());
}

void led_set (bool on)
{
    if(on)
        gpio_output_set(LED_IO_MASK, 0, 0, 0);
    else
        gpio_output_set(0, LED_IO_MASK, 0, 0);
}

bool led_is_set (void)
{
    return ((GPIO_REG_READ(GPIO_OUT_ADDRESS) & LED_IO_MASK)) ? true : false;
}
