/**
 * @file battery_monitor.c
 * @brief バッテリ監視モジュール実装（契約による設計の具体例）
 */
#include "battery_monitor.h"
#include "dbc_assert.h"

/** ADC 最大値（12bit） */
#define ADC_MAX 4095U
/** 基準電圧 [mV] */
#define VREF_MV 3300U
/** LOW→NORMAL 復帰時のヒステリシス幅 [mV]（チャタリング防止） */
#define HYSTERESIS_MV 50U

void battery_monitor_init(battery_monitor_t *ctx,
                          uint16_t low_mv,
                          uint16_t critical_mv,
                          uint16_t over_mv)
{
    /* 事前条件 */
    DBC_REQUIRE(ctx != 0);
    DBC_REQUIRE(critical_mv < low_mv);
    DBC_REQUIRE(low_mv < over_mv);

    if (ctx == 0) {
        return;
    }

    ctx->low_threshold_mv = low_mv;
    ctx->critical_threshold_mv = critical_mv;
    ctx->over_threshold_mv = over_mv;
    ctx->last_state = BATTERY_STATE_NORMAL;
    ctx->last_voltage_mv = 0;

    /* 事後条件 */
    DBC_ENSURE(ctx->last_state == BATTERY_STATE_NORMAL);
}

uint16_t battery_raw_to_mv(uint16_t raw_adc)
{
    /* 事前条件 */
    DBC_REQUIRE(raw_adc <= ADC_MAX);

    if (raw_adc > ADC_MAX) {
        return VREF_MV; /* 防御的: 範囲外は最大電圧として返す */
    }

    uint16_t voltage_mv = (uint16_t)((uint32_t)raw_adc * VREF_MV / ADC_MAX);

    /* 事後条件 */
    DBC_ENSURE(voltage_mv <= VREF_MV);

    return voltage_mv;
}

battery_state_t battery_evaluate(const battery_monitor_t *ctx,
                                 uint16_t voltage_mv)
{
    /* 事前条件 */
    DBC_REQUIRE(ctx != 0);
    DBC_REQUIRE(voltage_mv <= VREF_MV);

    if (ctx == 0) {
        return BATTERY_STATE_INVALID;
    }

    battery_state_t result;

    if (voltage_mv > ctx->over_threshold_mv) {
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

    /* 事後条件 */
    DBC_ENSURE(result != BATTERY_STATE_INVALID);

    return result;
}

battery_state_t battery_monitor_update(battery_monitor_t *ctx, uint16_t raw_adc)
{
    /* 事前条件 */
    DBC_REQUIRE(ctx != 0);
    DBC_REQUIRE(raw_adc <= ADC_MAX);

    if (ctx == 0) {
        return BATTERY_STATE_INVALID;
    }

    uint16_t voltage_mv = battery_raw_to_mv(raw_adc);
    battery_state_t state = battery_evaluate(ctx, voltage_mv);

    ctx->last_voltage_mv = voltage_mv;
    ctx->last_state = state;

    /* 事後条件 */
    DBC_ENSURE(ctx->last_state == state);
    /* 不変条件 */
    DBC_INVARIANT(ctx->last_voltage_mv <= VREF_MV);

    return state;
}
