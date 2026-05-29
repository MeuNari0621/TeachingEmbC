/**
 * @file battery_monitor.h
 * @brief バッテリ監視モジュール（契約による設計 + TDD のサンプル）
 *
 * 契約:
 *   - 事前条件: ADC 生値は 12bit 範囲 (0〜4095)
 *   - 事後条件: 電圧は 0〜3300 mV の範囲
 *   - 不変条件: 状態は定義された列挙値のいずれか
 */
#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** バッテリ状態 */
typedef enum {
    BATTERY_STATE_NORMAL = 0,  /**< 正常 */
    BATTERY_STATE_LOW,         /**< 低電圧警告 */
    BATTERY_STATE_CRITICAL,    /**< 危険（即時停止推奨） */
    BATTERY_STATE_OVERVOLTAGE, /**< 過電圧 */
    BATTERY_STATE_INVALID      /**< 無効入力（契約違反検出時） */
} battery_state_t;

/** バッテリ監視コンテキスト */
typedef struct {
    uint16_t low_threshold_mv;      /**< 低電圧閾値 [mV] */
    uint16_t critical_threshold_mv; /**< 危険閾値 [mV] */
    uint16_t over_threshold_mv;     /**< 過電圧閾値 [mV] */
    battery_state_t last_state;     /**< 直近の判定状態 */
    uint16_t last_voltage_mv;       /**< 直近の電圧値 [mV] */
} battery_monitor_t;

/**
 * @brief バッテリ監視コンテキストを初期化する
 * @param ctx           コンテキストへのポインタ
 * @param low_mv        低電圧閾値 [mV]
 * @param critical_mv   危険閾値 [mV]
 * @param over_mv       過電圧閾値 [mV]
 *
 * 事前条件: ctx != NULL
 * 事前条件: critical_mv < low_mv < over_mv
 * 事後条件: ctx->last_state == BATTERY_STATE_NORMAL
 */
void battery_monitor_init(battery_monitor_t *ctx,
                          uint16_t low_mv,
                          uint16_t critical_mv,
                          uint16_t over_mv);

/**
 * @brief ADC 生値を電圧 [mV] に変換する（純粋関数）
 * @param raw_adc ADC 生値（12bit, 0〜4095）
 * @return 電圧値 [mV]（0〜3300）
 *
 * 事前条件: raw_adc <= 4095
 * 事後条件: 戻り値 <= 3300
 */
uint16_t battery_raw_to_mv(uint16_t raw_adc);

/**
 * @brief 電圧値からバッテリ状態を判定する（純粋関数）
 * @param ctx       コンテキストへのポインタ（閾値参照用）
 * @param voltage_mv 電圧値 [mV]
 * @return バッテリ状態
 *
 * 事前条件: ctx != NULL
 * 事前条件: voltage_mv <= 3300
 * 事後条件: 戻り値は BATTERY_STATE_INVALID 以外の有効な状態
 */
battery_state_t battery_evaluate(const battery_monitor_t *ctx,
                                 uint16_t voltage_mv);

/**
 * @brief ADC 生値を受け取り、状態を更新する（オーケストレータ）
 * @param ctx       コンテキストへのポインタ
 * @param raw_adc   ADC 生値（12bit）
 * @return 判定されたバッテリ状態
 *
 * 事前条件: ctx != NULL
 * 事前条件: raw_adc <= 4095
 * 事後条件: ctx->last_state == 戻り値
 * 不変条件: ctx->last_voltage_mv <= 3300
 */
battery_state_t battery_monitor_update(battery_monitor_t *ctx, uint16_t raw_adc);

#ifdef __cplusplus
}
#endif

#endif /* BATTERY_MONITOR_H */
