/**
 * @file test_state_transition.cpp
 * @brief 純粋関数化した状態遷移ロジックのテスト
 */
#include "gtest/gtest.h"

extern "C" {
#include "temp_alarm_fsm.h"
}

class TempAlarmTransitionTest : public ::testing::Test {
protected:
    temp_alarm_fsm_t current;

    void SetUp() override {
        temp_alarm_fsm_init(&current);
    }
};

TEST_F(TempAlarmTransitionTest, StartEventReturnsMonitoringWithoutMutatingInput) {
    temp_alarm_event_t event = { TEMP_ALARM_EVENT_START, 0 };

    temp_alarm_fsm_t next = temp_alarm_transition(&current, &event);

    EXPECT_EQ(TEMP_ALARM_STATE_IDLE, current.state);
    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, next.state);
    EXPECT_EQ(1, next.sample_requested);
    EXPECT_EQ(0, next.alarm_led_on);
}

TEST_F(TempAlarmTransitionTest, HighTemperatureMovesToAlarmState) {
    temp_alarm_event_t start = { TEMP_ALARM_EVENT_START, 0 };
    temp_alarm_event_t adc_ready = { TEMP_ALARM_EVENT_ADC_READY, 4000 };

    current = temp_alarm_transition(&current, &start);
    temp_alarm_fsm_t next = temp_alarm_transition(&current, &adc_ready);

    EXPECT_EQ(TEMP_ALARM_STATE_ALARM, next.state);
    EXPECT_EQ(1, next.alarm_led_on);
    EXPECT_GT(next.last_temp_x10, TEMP_ALARM_THRESHOLD_X10);
    EXPECT_EQ(0, next.sample_requested);
}

TEST_F(TempAlarmTransitionTest, InvalidSampleMovesToSensorFault) {
    temp_alarm_event_t start = { TEMP_ALARM_EVENT_START, 0 };
    temp_alarm_event_t adc_ready = { TEMP_ALARM_EVENT_ADC_READY, 0 };

    current = temp_alarm_transition(&current, &start);
    temp_alarm_fsm_t next = temp_alarm_transition(&current, &adc_ready);

    EXPECT_EQ(TEMP_ALARM_STATE_SENSOR_FAULT, next.state);
    EXPECT_EQ(TEMP_ALARM_ERROR_X10, next.last_temp_x10);
    EXPECT_EQ(1, next.alarm_led_on);
}

TEST_F(TempAlarmTransitionTest, AckButtonReturnsAlarmToMonitoring) {
    temp_alarm_event_t ack = { TEMP_ALARM_EVENT_ACK_BUTTON, 0 };

    current.state = TEMP_ALARM_STATE_ALARM;
    current.alarm_led_on = 1;

    temp_alarm_fsm_t next = temp_alarm_transition(&current, &ack);

    EXPECT_EQ(TEMP_ALARM_STATE_MONITORING, next.state);
    EXPECT_EQ(0, next.alarm_led_on);
    EXPECT_EQ(TEMP_ALARM_STATE_ALARM, current.state);
}

TEST_F(TempAlarmTransitionTest, StopEventResetsAllFields) {
    temp_alarm_event_t stop = { TEMP_ALARM_EVENT_STOP, 0 };

    current.state = TEMP_ALARM_STATE_SENSOR_FAULT;
    current.last_temp_x10 = TEMP_ALARM_ERROR_X10;
    current.alarm_led_on = 1;
    current.sample_requested = 1;

    temp_alarm_fsm_t next = temp_alarm_transition(&current, &stop);

    EXPECT_EQ(TEMP_ALARM_STATE_IDLE, next.state);
    EXPECT_EQ(0, next.last_temp_x10);
    EXPECT_EQ(0, next.alarm_led_on);
    EXPECT_EQ(0, next.sample_requested);
}