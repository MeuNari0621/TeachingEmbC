/**
 * @file bad_port_adapter.c
 * @brief ❌ 悪い例のポートアダプタ前コード（教材用・ビルドはしない）
 */
#include "bad_port_adapter.h"

/*
 * void bad_temp_app_step(void) {
 * #if defined(TARGET_STM32)
 *     uint16_t raw = stm32_adc_read_blocking();
 *     if (raw > 3000) {
 *         stm32_gpio_write(ALARM_LED_PIN, 1);
 *     } else {
 *         stm32_gpio_write(ALARM_LED_PIN, 0);
 *     }
 * #elif defined(TARGET_NRF52)
 *     uint16_t raw = nrf_saadc_sample_blocking();
 *     if (raw > 3000) {
 *         nrf_gpio_pin_write(ALARM_LED_PIN, 1);
 *     } else {
 *         nrf_gpio_pin_write(ALARM_LED_PIN, 0);
 *     }
 * #endif
 * }
 *
 * 問題点:
 * - アプリケーションがターゲット別 API を直接知っている
 * - 新ターゲット追加のたびにアプリ側を修正する
 * - ホストテスト時に差し替える境界がない
 */