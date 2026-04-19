/**
 * @file bad_isr.c
 * @brief ❌ 悪い例の ISR コード（教材用・ビルドはしない）
 */
#include "bad_isr.h"

/*
 * void TIM2_IRQHandler(void) {
 *     uint16_t raw = hal_adc_read(TEMP_ADC_CHANNEL);       // ISR 内でADC読み取り
 *     int16_t temp = temperature_convert(raw);             // ISR 内でロジック実行
 *
 *     if (temp > TEMP_ALARM_THRESHOLD_X10) {
 *         hal_gpio_write(ALARM_LED_PIN, 1);                // ISR 内で副作用
 *         uart_printf("alarm\n");                        // 重い処理
 *     }
 *
 *     while (!adc_conversion_done()) {}                    // ブロッキング待ち
 * }
 *
 * void ADC1_IRQHandler(void) {
 *     // 取得値の検証、状態更新、LED制御を全部ここで実施
 * }
 *
 * void EXTI0_IRQHandler(void) {
 *     // ボタン確認で状態変更とGPIO制御を直接実施
 * }
 */