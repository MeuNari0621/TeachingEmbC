/**
 * @file bad_state_machine.c
 * @brief ❌ 悪い例の状態遷移コード（教材用・ビルドはしない）
 */
#include "bad_state_machine.h"

volatile uint8_t g_alarm_state = 0;
volatile uint8_t g_sample_due = 0;
volatile int16_t g_last_temp_x10 = 0;

/*
 * void bad_temp_alarm_task(void) {
 *     if (g_sample_due) {
 *         uint16_t raw = hal_adc_read(TEMP_ADC_CHANNEL);   // 副作用
 *         int16_t temp = temperature_convert(raw);         // ロジック
 *         g_last_temp_x10 = temp;                          // グローバル更新
 *
 *         if (raw == 0 || raw >= 4095) {
 *             g_alarm_state = 3;
 *             hal_gpio_write(ALARM_LED_PIN, 1);            // 副作用
 *         } else if (temp > TEMP_ALARM_THRESHOLD_X10) {
 *             g_alarm_state = 2;
 *             hal_gpio_write(ALARM_LED_PIN, 1);            // 副作用
 *         } else {
 *             g_alarm_state = 1;
 *             hal_gpio_write(ALARM_LED_PIN, 0);            // 副作用
 *         }
 *
 *         if (hal_gpio_read(ACK_BUTTON_PIN)) {             // 入力も直接読む
 *             g_alarm_state = 1;
 *         }
 *
 *         g_sample_due = 0;
 *     }
 * }
 */