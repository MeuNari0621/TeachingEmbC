/**
 * @file temp_monitor.c
 * @brief 温度監視モジュールの実装
 *
 * 副作用（HAL呼び出し）と純粋なロジックを明確に分離している。
 */
#include "temp_monitor.h"
#include "temperature.h"
#include "hal_adc.h"
#include "hal_gpio.h"

int16_t temp_monitor_execute(void) {
    /* 1. ADC から生値を読む（副作用） */
    uint16_t raw = hal_adc_read(TEMP_ADC_CHANNEL);

    /* 2. 入力のバリデーション（純粋関数） */
    if (!temperature_is_valid(raw)) {
        /* センサ異常: アラーム点灯して異常値を返す */
        hal_gpio_write(ALARM_LED_PIN, 1);
        return -9999;
    }

    /* 3. 温度変換（純粋関数） */
    int16_t temp = temperature_convert(raw);

    /* 4. 閾値チェック（純粋関数） */
    int over = temperature_is_over(temp, ALARM_THRESHOLD_X10);

    /* 5. LED 制御（副作用） */
    hal_gpio_write(ALARM_LED_PIN, (uint8_t)over);

    return temp;
}
