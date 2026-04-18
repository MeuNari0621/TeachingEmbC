/**
 * @file temperature.c
 * @brief 温度変換モジュールの実装
 */
#include "temperature.h"

int16_t temperature_convert(uint16_t raw_adc) {
    /* mV = raw * 3300 / 4095 */
    int32_t mv = (int32_t)raw_adc * 3300 / 4095;
    /* 簡易変換: temp_x10 = mv / 10 */
    /* ×10 で保持（例: 165 = 16.5℃） */
    int16_t temp_x10 = (int16_t)(mv / 10);
    return temp_x10;
}

int temperature_is_over(int16_t temp_x10, int16_t threshold_x10) {
    return (temp_x10 > threshold_x10) ? 1 : 0;
}

int temperature_is_valid(uint16_t raw_adc) {
    /* 0 や 4095 に張り付いている場合はセンサ異常と判断 */
    if (raw_adc == 0 || raw_adc >= 4095) {
        return 0;
    }
    return 1;
}
