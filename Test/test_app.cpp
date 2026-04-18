/**
 * @file test_temperature.cpp
 * @brief 温度変換モジュールのテスト
 *
 * temperature.c は純粋関数のみで構成されているため、
 * フェイクやモックなしで直接テストできる。
 * これが「テスト容易性」の好例。
 */
#include "gtest/gtest.h"

extern "C" {
#include "temperature.h"
}

/* ===== temperature_convert のテスト ===== */

class TemperatureConvertTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TemperatureConvertTest, ZeroRawReturnsZero) {
    /* ADC 0 → 0mV → 0℃ */
    EXPECT_EQ(0, temperature_convert(0));
}

TEST_F(TemperatureConvertTest, MidRangeValue) {
    /* ADC 2048 → 約 1650mV → 約 165 (約16.5℃) */
    int16_t result = temperature_convert(2048);
    EXPECT_GE(result, 160);
    EXPECT_LE(result, 170);
}

TEST_F(TemperatureConvertTest, MaxRawValue) {
    /* ADC 4095 → 3300mV → 330 (33.0℃) */
    int16_t result = temperature_convert(4095);
    EXPECT_EQ(330, result);
}

/* ===== temperature_is_over のテスト ===== */

class TemperatureIsOverTest : public ::testing::Test {};

TEST_F(TemperatureIsOverTest, BelowThreshold) {
    EXPECT_EQ(0, temperature_is_over(499, 500));
}

TEST_F(TemperatureIsOverTest, ExactlyAtThreshold) {
    EXPECT_EQ(0, temperature_is_over(500, 500));
}

TEST_F(TemperatureIsOverTest, AboveThreshold) {
    EXPECT_EQ(1, temperature_is_over(501, 500));
}

/* ===== temperature_is_valid のテスト ===== */

class TemperatureIsValidTest : public ::testing::Test {};

TEST_F(TemperatureIsValidTest, ZeroIsInvalid) {
    EXPECT_EQ(0, temperature_is_valid(0));
}

TEST_F(TemperatureIsValidTest, MaxIsInvalid) {
    EXPECT_EQ(0, temperature_is_valid(4095));
}

TEST_F(TemperatureIsValidTest, NormalValueIsValid) {
    EXPECT_EQ(1, temperature_is_valid(2048));
}

TEST_F(TemperatureIsValidTest, LowValidValue) {
    EXPECT_EQ(1, temperature_is_valid(1));
}

TEST_F(TemperatureIsValidTest, HighValidValue) {
    EXPECT_EQ(1, temperature_is_valid(4094));
}