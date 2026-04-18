/**
 * @file hal_adc.h
 * @brief ADC ハードウェア抽象化レイヤ（HAL）
 *
 * ハードウェアアクセスを関数ポインタで抽象化することで、
 * テスト時にフェイク実装に差し替え可能にする。
 */
#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

/**
 * @brief ADC から生の値を読み取る
 * @param channel ADC チャネル番号
 * @return 生の ADC 値（0-4095）
 *
 * 実機ではレジスタアクセス、テストではフェイク値を返す。
 */
uint16_t hal_adc_read(uint8_t channel);

/**
 * @brief ADC 初期化
 */
void hal_adc_init(void);

#endif /* HAL_ADC_H */
