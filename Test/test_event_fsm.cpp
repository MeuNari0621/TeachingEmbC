/**
 * @file test_event_fsm.cpp
 * @brief ISR ラッパーとイベント駆動アダプタのテスト
 */
#include "gtest/gtest.h"

extern "C" {
#include "temp_alarm_fsm.h"
}

class TempAlarmFsmTest : public ::testing::Test {
protected:
    temp_alarm_fsm_t fsm;

    void SetUp() override {
        temp_alarm_fsm_init(&fsm);
    }

    void startMonitoring() {
        temp_alarm_event_t event = { TEMP_ALARM_EVENT_START, 0 };
        temp_alarm_fsm_dispatch(&fsm, &event);
    }
};

TEST_F(TempAlarmFsmTest, InitStartsIdle) {
    EXPECT_EQ(TEMP_ALARM_STATE_IDLE, fsm.state);
    EXPECT_EQ(0, fsm.last_temp_x10);
    EXPECT_EQ(0, fsm.alarm_led_on);
    EXPECT_EQ(0, fsm.sample_requested);
}

TEST_F(TempAlarmFsmTest, StartEventMovesToMonitoringAndRequestsFirstSample) {
    startMonitoring();

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, fsm.state);
    EXPECT_EQ(1, fsm.sample_requested);
    EXPECT_EQ(0, fsm.alarm_led_on);
}

TEST_F(TempAlarmFsmTest, TimerInterruptRequestsSampleWithoutChangingState) {
    startMonitoring();
    fsm.sample_requested = 0;

    temp_alarm_fsm_on_timer_interrupt(&fsm);

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, fsm.state);
    EXPECT_EQ(1, fsm.sample_requested);
}

TEST_F(TempAlarmFsmTest, AdcInterruptNormalSampleKeepsMonitoringAndLedOff) {
    startMonitoring();

    temp_alarm_fsm_on_adc_interrupt(&fsm, 2048);

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, fsm.state);
    EXPECT_EQ(0, fsm.alarm_led_on);
    EXPECT_EQ(0, fsm.sample_requested);
    EXPECT_GE(fsm.last_temp_x10, 160);
    EXPECT_LE(fsm.last_temp_x10, 170);
}

TEST_F(TempAlarmFsmTest, AdcInterruptHighTemperatureTransitionsToAlarm) {
    startMonitoring();

    temp_alarm_fsm_on_adc_interrupt(&fsm, 4000);

    EXPECT_EQ(TEMP_ALARM_STATE_ALARM, fsm.state);
    EXPECT_EQ(1, fsm.alarm_led_on);
    EXPECT_GT(fsm.last_temp_x10, TEMP_ALARM_THRESHOLD_X10);
}

TEST_F(TempAlarmFsmTest, AckInterruptReturnsFromAlarmToMonitoring) {
    startMonitoring();
    temp_alarm_fsm_on_adc_interrupt(&fsm, 4000);

    temp_alarm_fsm_on_ack_interrupt(&fsm);

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, fsm.state);
    EXPECT_EQ(0, fsm.alarm_led_on);
}

TEST_F(TempAlarmFsmTest, InvalidAdcSampleTransitionsToSensorFault) {
    startMonitoring();

    temp_alarm_fsm_on_adc_interrupt(&fsm, 0);

    EXPECT_EQ(TEMP_ALARM_STATE_SENSOR_FAULT, fsm.state);
    EXPECT_EQ(TEMP_ALARM_ERROR_X10, fsm.last_temp_x10);
    EXPECT_EQ(1, fsm.alarm_led_on);
}

TEST_F(TempAlarmFsmTest, FaultCanRecoverOnNextValidSample) {
    startMonitoring();
    temp_alarm_fsm_on_adc_interrupt(&fsm, 0);

    temp_alarm_fsm_on_timer_interrupt(&fsm);
    temp_alarm_fsm_on_adc_interrupt(&fsm, 2000);

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, fsm.state);
    EXPECT_EQ(0, fsm.alarm_led_on);
    EXPECT_EQ(0, fsm.sample_requested);
    EXPECT_GT(fsm.last_temp_x10, 0);
}

TEST_F(TempAlarmFsmTest, StopEventResetsStateMachine) {
    temp_alarm_event_t stop_event = { TEMP_ALARM_EVENT_STOP, 0 };

    startMonitoring();
    temp_alarm_fsm_on_adc_interrupt(&fsm, 4000);
    temp_alarm_fsm_dispatch(&fsm, &stop_event);

    EXPECT_EQ(TEMP_ALARM_STATE_IDLE, fsm.state);
    EXPECT_EQ(0, fsm.last_temp_x10);
    EXPECT_EQ(0, fsm.alarm_led_on);
    EXPECT_EQ(0, fsm.sample_requested);
}