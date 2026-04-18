/**
 * @file hal_adc.c
 * @brief ADC HAL の実機実装
 *
 * ターゲット環境ではレジスタアクセスを行う。
 * ホストテスト時はこのファイルの代わりに FFF フェイクを使う。
 */
#include "hal_adc.h"

/* 実機ではここでレジスタアクセスを行う */
/* 例: #include "stm32f4xx_hal.h" */

void hal_adc_init(void) {
    /* 実機: ADC ペリフェラル初期化 */
    /* HAL_ADC_Init(&hadc1); */
}

uint16_t hal_adc_read(uint8_t channel) {
    (void)channel;
    /* 実機: レジスタから ADC 値を読み取り */
    /* HAL_ADC_Start(&hadc1); */
    /* HAL_ADC_PollForConversion(&hadc1, 100); */
    /* return HAL_ADC_GetValue(&hadc1); */
    return 0; /* ホストビルド用スタブ */
}
