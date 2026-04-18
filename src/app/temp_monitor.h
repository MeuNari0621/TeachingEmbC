/**
 * @file temp_monitor.h
 * @brief 温度監視モジュール（副作用を含む）
 *
 * HAL を介してセンサを読み取り、閾値超過時にアラームを制御する。
 * HAL 関数を FFF でフェイク化することでテスト可能にする。
 */
#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H

#include <stdint.h>

/** アラームLED のピン番号 */
#define ALARM_LED_PIN 13
/** 温度センサの ADC チャネル */
#define TEMP_ADC_CHANNEL 0
/** アラーム閾値（×10, 例: 300 = 30.0℃） */
#define ALARM_THRESHOLD_X10 300

/**
 * @brief 温度を監視しアラームを制御する
 * @return 測定温度（×10 の整数表現）。センサ異常時は -9999
 *
 * 処理フロー:
 * 1. ADC から生値を読む（副作用: ハードウェアアクセス）
 * 2. 生値を温度に変換する（純粋関数）
 * 3. 閾値と比較する（純粋関数）
 * 4. LED を制御する（副作用: ハードウェアアクセス）
 */
int16_t temp_monitor_execute(void);

#endif /* TEMP_MONITOR_H */
