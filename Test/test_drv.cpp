/**
 * @file test_temp_monitor.cpp
 * @brief 温度監視モジュールのテスト（FFF でHALをフェイク化）
 *
 * temp_monitor.c は HAL 関数を呼び出す（副作用あり）。
 * FFF を使って HAL 関数をフェイクに差し替え、
 * ホスト環境でも副作用のあるコードをテスト可能にする。
 */
#include "gtest/gtest.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

extern "C" {
#include "temp_monitor.h"
#include "temperature.h"
#include "hal_adc.h"
#include "hal_gpio.h"
}

/* HAL 関数をフェイクに差し替え */
FAKE_VALUE_FUNC(uint16_t, hal_adc_read, uint8_t);
FAKE_VOID_FUNC(hal_adc_init);
FAKE_VOID_FUNC(hal_gpio_write, uint8_t, uint8_t);
FAKE_VALUE_FUNC(uint8_t, hal_gpio_read, uint8_t);

class TempMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        /* 各テスト前にフェイクをリセット */
        RESET_FAKE(hal_adc_read);
        RESET_FAKE(hal_adc_init);
        RESET_FAKE(hal_gpio_write);
        RESET_FAKE(hal_gpio_read);
        FFF_RESET_HISTORY();
    }
};

/* --- 正常系テスト --- */

TEST_F(TempMonitorTest, NormalTemperature_LedOff) {
    /* ADC 1000 → 約 80 (8.0℃) → 閾値 ALARM_THRESHOLD_X10(300) 未満 → LED OFF */
    hal_adc_read_fake.return_val = 1000;

    int16_t result = temp_monitor_execute();

    /* 温度が返る */
    EXPECT_GE(result, 0);
    EXPECT_LT(result, ALARM_THRESHOLD_X10);

    /* GPIO に 0 (OFF) が書かれる */
    EXPECT_EQ(hal_gpio_write_fake.call_count, 1);
    EXPECT_EQ(hal_gpio_write_fake.arg0_val, ALARM_LED_PIN);
    EXPECT_EQ(hal_gpio_write_fake.arg1_val, 0);
}

TEST_F(TempMonitorTest, HighTemperature_LedOn) {
    /* ADC 4000 → 高温 → 閾値超過 → LED ON */
    hal_adc_read_fake.return_val = 4000;

    int16_t result = temp_monitor_execute();

    EXPECT_GT(result, ALARM_THRESHOLD_X10);
    EXPECT_EQ(hal_gpio_write_fake.call_count, 1);
    EXPECT_EQ(hal_gpio_write_fake.arg0_val, ALARM_LED_PIN);
    EXPECT_EQ(hal_gpio_write_fake.arg1_val, 1);
}

/* --- 異常系テスト --- */

TEST_F(TempMonitorTest, SensorDisconnected_ReturnsError) {
    /* ADC 0 → センサ断線 */
    hal_adc_read_fake.return_val = 0;

    int16_t result = temp_monitor_execute();

    EXPECT_EQ(result, -9999);
    /* アラーム LED が点灯する */
    EXPECT_EQ(hal_gpio_write_fake.arg1_val, 1);
}

TEST_F(TempMonitorTest, SensorShorted_ReturnsError) {
    /* ADC 4095 → センサ短絡 */
    hal_adc_read_fake.return_val = 4095;

    int16_t result = temp_monitor_execute();

    EXPECT_EQ(result, -9999);
    EXPECT_EQ(hal_gpio_write_fake.arg1_val, 1);
}

/* --- HAL 呼び出し順序テスト --- */

TEST_F(TempMonitorTest, CallOrder_ReadThenWrite) {
    hal_adc_read_fake.return_val = 2000;

    temp_monitor_execute();

    /* ADC read が先、GPIO write が後 */
    EXPECT_EQ(hal_adc_read_fake.call_count, 1);
    EXPECT_EQ(hal_gpio_write_fake.call_count, 1);
}