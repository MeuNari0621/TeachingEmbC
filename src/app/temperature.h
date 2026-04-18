/**
 * @file temperature.h
 * @brief 温度変換モジュール（純粋関数）
 *
 * ADC 生値から温度への変換を行う純粋関数群。
 * 副作用がないため、テストが非常に容易。
 */
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <stdint.h>

/**
 * @brief ADC 生値を摂氏温度に変換する（純粋関数）
 * @param raw_adc ADC 生値（0〜4095、12bit）
 * @return 摂氏温度（×10 の整数表現、例: 256 = 25.6℃）
 *
 * 変換式: temp = (raw_adc * 3300 / 4095 - 500) / 10
 * ※ LM35 センサ想定: 10mV/℃、0℃=0mV
 * ※ ×10 で保持し整数演算のみで精度を確保
 */
int16_t temperature_convert(uint16_t raw_adc);

/**
 * @brief 温度が閾値を超えているかチェック（純粋関数）
 * @param temp_x10 温度（×10 の整数表現）
 * @param threshold_x10 閾値（×10 の整数表現）
 * @return 1: 超過, 0: 正常
 */
int temperature_is_over(int16_t temp_x10, int16_t threshold_x10);

/**
 * @brief 温度値のバリデーション（純粋関数）
 * @param raw_adc ADC 生値
 * @return 1: 有効, 0: 無効（センサ断線等）
 */
int temperature_is_valid(uint16_t raw_adc);

#endif /* TEMPERATURE_H */
