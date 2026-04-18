/**
 * @file bad_temp.h
 * @brief ❌ 悪い例: テスト困難な温度監視コード
 *
 * この例は「ベストプラクティス導入前」を示すためのものです。
 * - ハードウェアアクセスとロジックが混在
 * - グローバル変数に依存
 * - 純粋関数がなく、全てが副作用を持つ
 */
#ifndef BAD_TEMP_H
#define BAD_TEMP_H

#include <stdint.h>

/** グローバル状態（❌ テスト困難の原因） */
extern volatile uint16_t g_last_raw_adc;
extern volatile int16_t g_last_temperature;
extern volatile uint8_t g_alarm_active;

/**
 * @brief 温度を読み取り、グローバル変数に格納し、アラームを制御
 *
 * ❌ 問題点:
 * - 直接レジスタにアクセス（ホストでテスト不可）
 * - グローバル変数を変更（副作用が多い）
 * - ロジックとハードウェアアクセスが混在
 * - 戻り値がなく結果はグローバル変数経由
 */
void bad_read_and_check_temp(void);

#endif /* BAD_TEMP_H */
