/**
 * @file hal_gpio.c
 * @brief GPIO HAL の実機実装
 */
#include "hal_gpio.h"

void hal_gpio_write(uint8_t pin, uint8_t state) {
    (void)pin;
    (void)state;
    /* 実機: GPIO レジスタに書き込み */
}

uint8_t hal_gpio_read(uint8_t pin) {
    (void)pin;
    /* 実機: GPIO レジスタから読み取り */
    return 0;
}
