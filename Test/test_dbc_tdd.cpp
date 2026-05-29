/**
 * @file test_dbc_tdd.cpp
 * @brief 契約による設計 + TDD のテスト（第14〜16章の教材コード）
 *
 * テスト駆動開発の Red-Green-Refactor サイクルに沿って、
 * バッテリ監視モジュールを検証する。
 */
#include <gtest/gtest.h>

extern "C" {
#include "battery_monitor.h"
#include "dbc_assert.h"

/* テスト用: dbc_assert.c で定義されたヘルパ */
int dbc_get_violation_count(void);
void dbc_reset_violations(void);
const char *dbc_get_last_violation_type(void);
}

// ============================================================
// フィクスチャ: バッテリ監視コンテキストの共通初期化
// ============================================================
class BatteryMonitorTest : public ::testing::Test {
protected:
    battery_monitor_t ctx_;

    void SetUp() override {
        dbc_reset_violations();
        // 閾値: critical=2400mV, low=2700mV, over=3100mV
        battery_monitor_init(&ctx_, 2700, 2400, 3100);
    }
};

// ============================================================
// 第14章: 契約による設計 — 事前条件・事後条件・不変条件の検証
// ============================================================

// --- ADC → 電圧変換の事前条件・事後条件 ---

TEST(BatteryRawToMv, ZeroInputReturnsZero) {
    EXPECT_EQ(0, battery_raw_to_mv(0));
}

TEST(BatteryRawToMv, MaxInputReturnsVref) {
    EXPECT_EQ(3300, battery_raw_to_mv(4095));
}

TEST(BatteryRawToMv, MidpointInput) {
    // 2048 / 4095 * 3300 ≈ 1650
    uint16_t result = battery_raw_to_mv(2048);
    EXPECT_GE(result, 1649);
    EXPECT_LE(result, 1651);
}

TEST(BatteryRawToMv, PostconditionVoltageInRange) {
    // 全範囲でも事後条件（戻り値 <= 3300）を満たす
    for (uint16_t adc = 0; adc <= 4095; adc += 100) {
        uint16_t mv = battery_raw_to_mv(adc);
        EXPECT_LE(mv, 3300) << "adc=" << adc;
    }
}

// --- 初期化の契約 ---

TEST_F(BatteryMonitorTest, InitSetsNormalState) {
    EXPECT_EQ(BATTERY_STATE_NORMAL, ctx_.last_state);
    EXPECT_EQ(0, dbc_get_violation_count());
}

TEST(BatteryMonitorInit, PreconditionViolation_CriticalNotLessThanLow) {
    battery_monitor_t ctx;
    dbc_reset_violations();
    // critical(2800) >= low(2700) → 事前条件違反
    battery_monitor_init(&ctx, 2700, 2800, 3100);
    EXPECT_GT(dbc_get_violation_count(), 0);
    EXPECT_STREQ("PRE", dbc_get_last_violation_type());
}

// ============================================================
// 第15章: テスト駆動開発 — Red-Green-Refactor
// ============================================================

// --- Green: 正常系の各状態判定 ---

TEST_F(BatteryMonitorTest, NormalVoltage) {
    // 2800mV: critical(2400) < low(2700) < 2800 < over(3100)
    battery_state_t state = battery_evaluate(&ctx_, 2800);
    EXPECT_EQ(BATTERY_STATE_NORMAL, state);
}

TEST_F(BatteryMonitorTest, LowVoltage) {
    // 2600mV: critical(2400) < 2600 <= low(2700)
    battery_state_t state = battery_evaluate(&ctx_, 2600);
    EXPECT_EQ(BATTERY_STATE_LOW, state);
}

TEST_F(BatteryMonitorTest, CriticalVoltage) {
    // 2400mV: 2400 <= critical(2400)
    battery_state_t state = battery_evaluate(&ctx_, 2400);
    EXPECT_EQ(BATTERY_STATE_CRITICAL, state);
}

TEST_F(BatteryMonitorTest, Overvoltage) {
    // 3200mV: > over(3100)
    battery_state_t state = battery_evaluate(&ctx_, 3200);
    EXPECT_EQ(BATTERY_STATE_OVERVOLTAGE, state);
}

// --- 境界値テスト ---

TEST_F(BatteryMonitorTest, BoundaryAtLowThreshold) {
    // 2700mV ちょうど → LOW（<= low_threshold）
    EXPECT_EQ(BATTERY_STATE_LOW, battery_evaluate(&ctx_, 2700));
}

TEST_F(BatteryMonitorTest, BoundaryAboveLowThreshold) {
    // 2701mV → NORMAL
    EXPECT_EQ(BATTERY_STATE_NORMAL, battery_evaluate(&ctx_, 2701));
}

TEST_F(BatteryMonitorTest, BoundaryAtCriticalThreshold) {
    // 2400mV ちょうど → CRITICAL
    EXPECT_EQ(BATTERY_STATE_CRITICAL, battery_evaluate(&ctx_, 2400));
}

TEST_F(BatteryMonitorTest, BoundaryAboveCriticalThreshold) {
    // 2401mV → LOW
    EXPECT_EQ(BATTERY_STATE_LOW, battery_evaluate(&ctx_, 2401));
}

TEST_F(BatteryMonitorTest, BoundaryAtOverThreshold) {
    // 3100mV ちょうど → NORMAL（> で判定なので 3100 は超えていない）
    EXPECT_EQ(BATTERY_STATE_NORMAL, battery_evaluate(&ctx_, 3100));
}

TEST_F(BatteryMonitorTest, BoundaryAboveOverThreshold) {
    // 3101mV → OVERVOLTAGE
    EXPECT_EQ(BATTERY_STATE_OVERVOLTAGE, battery_evaluate(&ctx_, 3101));
}

// ============================================================
// 第16章: 実装 — オーケストレータの統合テスト
// ============================================================

TEST_F(BatteryMonitorTest, UpdateFromAdc_Normal) {
    // ADC=3500 → 3500*3300/4095 ≈ 2820mV → NORMAL
    battery_state_t state = battery_monitor_update(&ctx_, 3500);
    EXPECT_EQ(BATTERY_STATE_NORMAL, state);
    EXPECT_EQ(state, ctx_.last_state);
    EXPECT_LE(ctx_.last_voltage_mv, 3300);
}

TEST_F(BatteryMonitorTest, UpdateFromAdc_Low) {
    // ADC=3200 → 3200*3300/4095 ≈ 2578mV → LOW
    battery_state_t state = battery_monitor_update(&ctx_, 3200);
    EXPECT_EQ(BATTERY_STATE_LOW, state);
    EXPECT_EQ(state, ctx_.last_state);
}

TEST_F(BatteryMonitorTest, UpdateFromAdc_Critical) {
    // ADC=2800 → 2800*3300/4095 ≈ 2255mV → CRITICAL
    battery_state_t state = battery_monitor_update(&ctx_, 2800);
    EXPECT_EQ(BATTERY_STATE_CRITICAL, state);
    EXPECT_EQ(state, ctx_.last_state);
}

TEST_F(BatteryMonitorTest, UpdateFromAdc_Overvoltage) {
    // ADC=4095 → 3300mV → OVERVOLTAGE (> over=3100)
    battery_state_t state = battery_monitor_update(&ctx_, 4095);
    EXPECT_EQ(BATTERY_STATE_OVERVOLTAGE, state);
    EXPECT_EQ(state, ctx_.last_state);
}

TEST_F(BatteryMonitorTest, UpdateFromAdc_ZeroInput) {
    // ADC=0 → 0mV → CRITICAL
    battery_state_t state = battery_monitor_update(&ctx_, 0);
    EXPECT_EQ(BATTERY_STATE_CRITICAL, state);
}

// --- 不変条件の検証: 連続呼び出しでも電圧が範囲内 ---

TEST_F(BatteryMonitorTest, InvariantVoltageRange) {
    const uint16_t test_values[] = {0, 100, 1000, 2048, 3000, 4000, 4095};
    for (auto adc : test_values) {
        battery_monitor_update(&ctx_, adc);
        EXPECT_LE(ctx_.last_voltage_mv, 3300) << "adc=" << adc;
    }
    EXPECT_EQ(0, dbc_get_violation_count());
}

// --- NULLポインタの防御的処理 ---

TEST(BatteryMonitorDefensive, EvaluateNullReturnsInvalid) {
    dbc_reset_violations();
    battery_state_t state = battery_evaluate(nullptr, 2500);
    EXPECT_EQ(BATTERY_STATE_INVALID, state);
    EXPECT_GT(dbc_get_violation_count(), 0);
}

TEST(BatteryMonitorDefensive, UpdateNullReturnsInvalid) {
    dbc_reset_violations();
    battery_state_t state = battery_monitor_update(nullptr, 2000);
    EXPECT_EQ(BATTERY_STATE_INVALID, state);
    EXPECT_GT(dbc_get_violation_count(), 0);
}
