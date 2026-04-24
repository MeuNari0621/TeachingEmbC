/**
 * @file hal_gpio.h
 * @brief GPIO ハードウェア抽象化レイヤ（HAL）
 *
 * 下回りは AUTOSAR Classic Platform 風の Port / Dio ドライバへ委譲する。
 */
#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>

/**
 * @brief GPIO ピンの状態を設定する
 * @param pin ピン番号
 * @param state 1=High, 0=Low
 */
void hal_gpio_write(uint8_t pin, uint8_t state);

/**
 * @brief GPIO ピンの状態を読み取る
 * @param pin ピン番号
 * @return 1=High, 0=Low
 */
uint8_t hal_gpio_read(uint8_t pin);

#endif /* HAL_GPIO_H */
