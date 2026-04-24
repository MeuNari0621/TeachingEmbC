/**
 * @file hal_adc.h
 * @brief ADC ハードウェア抽象化レイヤ（HAL）
 *
 * アプリケーション層からは従来どおり HAL として見せつつ、
 * 下回りは AUTOSAR Classic Platform 風の ADC ドライバへ委譲する。
 */
#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

/**
 * @brief ADC から生の値を読み取る
 * @param channel ADC チャネル番号
 * @return 生の ADC 値（0-4095）
 *
 * 実装側では AUTOSAR 風 MCAL の `Adc_StartGroupConversion()` /
 * `Adc_ReadGroup()` を使って値を取得する。
 */
uint16_t hal_adc_read(uint8_t channel);

/**
 * @brief ADC 初期化
 */
void hal_adc_init(void);

#endif /* HAL_ADC_H */
