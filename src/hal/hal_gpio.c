/**
 * @file hal_gpio.c
 * @brief GPIO HAL の実装
 *
 * アプリ層には HAL を維持しつつ、下回りは AUTOSAR Classic Platform
 * 風の Port / Dio ドライバへ橋渡しする。
 */
#include "hal_gpio.h"

#include "Dio.h"
#include "Port.h"

static void hal_gpio_ensure_initialized(void) {
    static uint8_t initialized = 0u;
    static const Port_ConfigType port_config = {
        13u,
        STD_LOW
    };

    if (initialized != 0u) {
        return;
    }

    Port_Init(&port_config);
    initialized = 1u;
}

void hal_gpio_write(uint8_t pin, uint8_t state) {
    hal_gpio_ensure_initialized();
    Dio_WriteChannel(pin, (state != 0u) ? STD_HIGH : STD_LOW);
}

uint8_t hal_gpio_read(uint8_t pin) {
    hal_gpio_ensure_initialized();
    return (uint8_t)Dio_ReadChannel(pin);
}
