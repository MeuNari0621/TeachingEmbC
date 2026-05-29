/**
 * @file test_object_oriented.cpp
 * @brief Cでのオブジェクト指向風サンプルをGoogleTestで検証する
 */
#include "gtest/gtest.h"

extern "C" {
#include "oo_temp_monitor.h"
}

namespace {

struct FakeAdc {
    uint16_t next_raw;
    int read_count;
};

struct FakeLed {
    uint8_t last_state;
    int write_count;
};

uint16_t fake_adc_read(void *context) {
    FakeAdc *adc = static_cast<FakeAdc *>(context);
    adc->read_count += 1;
    return adc->next_raw;
}

void fake_led_write(void *context, uint8_t state) {
    FakeLed *led = static_cast<FakeLed *>(context);
    led->last_state = state;
    led->write_count += 1;
}

class ObjectOrientedTempMonitorTest : public ::testing::Test {
protected:
    oo_adc_port_t make_adc_port(FakeAdc &adc) {
        return oo_adc_port_t{fake_adc_read, &adc};
    }

    oo_gpio_port_t make_led_port(FakeLed &led) {
        return oo_gpio_port_t{fake_led_write, &led};
    }
};

TEST_F(ObjectOrientedTempMonitorTest, InitStartsWithAlarmCleared) {
    FakeAdc adc{2048, 0};
    FakeLed led{1, 0};
    oo_temp_monitor_t monitor;

    oo_temp_monitor_init(&monitor, make_adc_port(adc), make_led_port(led), 300);

    EXPECT_EQ(0, oo_temp_monitor_alarm_active(&monitor));
    EXPECT_EQ(OO_TEMP_MONITOR_SENSOR_ERROR, oo_temp_monitor_last_temp(&monitor));
}

TEST_F(ObjectOrientedTempMonitorTest, NormalTemperatureKeepsAlarmOff) {
    FakeAdc adc{2000, 0};
    FakeLed led{1, 0};
    oo_temp_monitor_t monitor;

    oo_temp_monitor_init(&monitor, make_adc_port(adc), make_led_port(led), 300);

    int16_t result = oo_temp_monitor_run(&monitor);

    EXPECT_GE(result, 0);
    EXPECT_LT(result, 300);
    EXPECT_EQ(result, oo_temp_monitor_last_temp(&monitor));
    EXPECT_EQ(0, oo_temp_monitor_alarm_active(&monitor));
    EXPECT_EQ(1, adc.read_count);
    EXPECT_EQ(1, led.write_count);
    EXPECT_EQ(0, led.last_state);
}

TEST_F(ObjectOrientedTempMonitorTest, HighTemperatureTurnsAlarmOn) {
    FakeAdc adc{4000, 0};
    FakeLed led{0, 0};
    oo_temp_monitor_t monitor;

    oo_temp_monitor_init(&monitor, make_adc_port(adc), make_led_port(led), 300);

    int16_t result = oo_temp_monitor_run(&monitor);

    EXPECT_GT(result, 300);
    EXPECT_EQ(1, oo_temp_monitor_alarm_active(&monitor));
    EXPECT_EQ(1, led.last_state);
}

TEST_F(ObjectOrientedTempMonitorTest, InvalidSampleReturnsErrorAndLightsAlarm) {
    FakeAdc adc{0, 0};
    FakeLed led{0, 0};
    oo_temp_monitor_t monitor;

    oo_temp_monitor_init(&monitor, make_adc_port(adc), make_led_port(led), 300);

    int16_t result = oo_temp_monitor_run(&monitor);

    EXPECT_EQ(OO_TEMP_MONITOR_SENSOR_ERROR, result);
    EXPECT_EQ(OO_TEMP_MONITOR_SENSOR_ERROR, oo_temp_monitor_last_temp(&monitor));
    EXPECT_EQ(1, oo_temp_monitor_alarm_active(&monitor));
    EXPECT_EQ(1, led.last_state);
}

TEST_F(ObjectOrientedTempMonitorTest, TwoInstancesCanUseDifferentPortsAndThresholds) {
    FakeAdc room_adc{2000, 0};
    FakeLed room_led{1, 0};
    oo_temp_monitor_t room_monitor;
    FakeAdc hot_plate_adc{3800, 0};
    FakeLed hot_plate_led{0, 0};
    oo_temp_monitor_t hot_plate_monitor;

    oo_temp_monitor_init(&room_monitor,
                         make_adc_port(room_adc),
                         make_led_port(room_led),
                         300);
    oo_temp_monitor_init(&hot_plate_monitor,
                         make_adc_port(hot_plate_adc),
                         make_led_port(hot_plate_led),
                         250);

    EXPECT_LT(oo_temp_monitor_run(&room_monitor), 300);
    EXPECT_GT(oo_temp_monitor_run(&hot_plate_monitor), 250);

    EXPECT_EQ(0, room_led.last_state);
    EXPECT_EQ(1, hot_plate_led.last_state);
    EXPECT_EQ(1, room_adc.read_count);
    EXPECT_EQ(1, hot_plate_adc.read_count);
}

TEST_F(ObjectOrientedTempMonitorTest, MissingPortsReturnsErrorWithoutTouchingHardware) {
    oo_temp_monitor_t monitor;
    oo_adc_port_t adc_port = {0, 0};
    oo_gpio_port_t led_port = {0, 0};

    oo_temp_monitor_init(&monitor, adc_port, led_port, 300);

    EXPECT_EQ(OO_TEMP_MONITOR_SENSOR_ERROR, oo_temp_monitor_run(&monitor));
    EXPECT_EQ(0, oo_temp_monitor_alarm_active(&monitor));
    EXPECT_EQ(OO_TEMP_MONITOR_SENSOR_ERROR, oo_temp_monitor_last_temp(&monitor));
}

}  // namespace
