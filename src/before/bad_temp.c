/**
 * @file bad_temp.c
 * @brief ❌ 悪い例の実装（教材用・ビルドはしない）
 *
 * このファイルはベストプラクティス導入前のコード例を示すためのものです。
 * 直接ハードウェアレジスタにアクセスするため、ホストではビルドしません。
 * 教材の「Before」セクションで参照します。
 */
#include "bad_temp.h"

/* グローバル変数 */
volatile uint16_t g_last_raw_adc = 0;
volatile int16_t g_last_temperature = 0;
volatile uint8_t g_alarm_active = 0;

/*
 * ❌ 実装例（擬似コード — ターゲット専用ヘッダに依存するためコメントで記述）
 *
 * void bad_read_and_check_temp(void) {
 *     // 直接ハードウェアレジスタにアクセス → ホストで実行不可
 *     ADC1->CR2 |= ADC_CR2_SWSTART;
 *     while (!(ADC1->SR & ADC_SR_EOC)) {}
 *     g_last_raw_adc = ADC1->DR;
 *
 *     // ロジックとハードウェアが混在
 *     int32_t mv = (int32_t)g_last_raw_adc * 3300 / 4095;
 *     g_last_temperature = (int16_t)(mv / 10);
 *
 *     // 閾値チェックと GPIO 制御が一体化
 *     if (g_last_temperature > 500) {
 *         GPIOA->ODR |= (1 << 13);  // 直接レジスタアクセス
 *         g_alarm_active = 1;
 *     } else {
 *         GPIOA->ODR &= ~(1 << 13);
 *         g_alarm_active = 0;
 *     }
 * }
 */
