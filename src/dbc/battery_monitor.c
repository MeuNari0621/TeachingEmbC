/**
 * @file battery_monitor.c
 * @brief バッテリ監視モジュール実装
 *
 * 契約（事前条件・事後条件・不変条件）はヘッダの Doxygen コメントに文書化し、
 * テストコードで検証する。本番コードには防御的プログラミングのみを残す。
 */
#include "battery_monitor.h"

/** ADC 最大値（12bit） */
#define ADC_MAX 4095U
/** 基準電圧 [mV] */
#define VREF_MV 3300U
/** LOW→NORMAL 復帰時のヒステリシス幅 [mV]（チャタリング防止） */
#define HYSTERESIS_MV 50U
/** 過電圧ラッチの解除マージン [mV]（誤探知による解除防止） */
#define OVERVOLTAGE_LATCH_MARGIN_MV 100U

void battery_monitor_init(battery_monitor_t *ctx,
                          uint16_t low_mv,
                          uint16_t critical_mv,
                          uint16_t over_mv)
{
    /* QAC-B add */
    uint8_t null_flg = 0;

    /* 防御的処理: NULLポインタ */
    /* QAC-A2 */
    if (ctx == 0) {
        /* Do Nothing */
    }else{     
        ctx->low_threshold_mv = low_mv;
        ctx->critical_threshold_mv = critical_mv;
        ctx->over_threshold_mv = over_mv;
        ctx->last_state = BATTERY_STATE_NORMAL;
        ctx->last_voltage_mv = 0;
    }
}

uint16_t battery_raw_to_mv(uint16_t raw_adc)
{
    /* 防御的処理: 範囲外は最大電圧として返す */
    if (raw_adc > ADC_MAX) {
        return VREF_MV;
    }

    uint16_t voltage_mv = (uint16_t)((uint32_t)raw_adc * VREF_MV / ADC_MAX);

    return voltage_mv;
}

battery_state_t battery_evaluate(const battery_monitor_t *ctx,
                                 uint16_t voltage_mv)
{
    /* 防御的処理: NULLポインタ */
    if (ctx == 0) {
        return BATTERY_STATE_INVALID;
    }

    battery_state_t result;

    if (voltage_mv > ctx->over_threshold_mv) {
        result = BATTERY_STATE_OVERVOLTAGE;
    } else if ((ctx->last_state == BATTERY_STATE_OVERVOLTAGE) &&
               (voltage_mv > (uint16_t)(ctx->over_threshold_mv - OVERVOLTAGE_LATCH_MARGIN_MV))) {
        /* 過電圧ラッチ: マージン内では OVERVOLTAGE を保持し、誤探知で解除されないようにする */
        result = BATTERY_STATE_OVERVOLTAGE;
    } else if (voltage_mv <= ctx->critical_threshold_mv) {
        result = BATTERY_STATE_CRITICAL;
    } else if (voltage_mv <= ctx->low_threshold_mv) {
        result = BATTERY_STATE_LOW;
    } else if ((ctx->last_state == BATTERY_STATE_LOW) &&
               (voltage_mv <= (uint16_t)(ctx->low_threshold_mv + HYSTERESIS_MV))) {
        /* ヒステリシス: LOW から NORMAL への復帰にはマージンを設け、チャタリングを防ぐ */
        result = BATTERY_STATE_LOW;
    } else {
        result = BATTERY_STATE_NORMAL;
    }

    return result;
}

battery_state_t battery_monitor_update(battery_monitor_t *ctx, uint16_t raw_adc)
{
    /* 防御的処理: NULLポインタ */
    if (ctx == 0) {
        return BATTERY_STATE_INVALID;
    }

    uint16_t voltage_mv = battery_raw_to_mv(raw_adc);
    battery_state_t state = battery_evaluate(ctx, voltage_mv);

    ctx->last_voltage_mv = voltage_mv;
    ctx->last_state = state;

    /* QAC-A add */
    return state;
}
