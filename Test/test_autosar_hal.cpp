#include "gtest/gtest.h"

extern "C" {
#include "Adc.h"
#include "Dio.h"
#include "Port.h"
#include "hal_adc.h"
#include "hal_gpio.h"
}

namespace {
constexpr Adc_GroupType kTemperatureGroup = 0u;
constexpr Port_PinType kAlarmPin = 13u;
}  // namespace

class AutosarHalTest : public ::testing::Test {
protected:
    void SetUp() override {
        const Adc_ConfigType adc_config = {kTemperatureGroup, 0u};
        const Port_ConfigType port_config = {kAlarmPin, STD_LOW};
        Adc_Init(&adc_config);
        Port_Init(&port_config);
    }
};

TEST_F(AutosarHalTest, DioReflectsWrittenLevel) {
    Dio_WriteChannel(kAlarmPin, STD_HIGH);

    EXPECT_EQ(STD_HIGH, Dio_ReadChannel(kAlarmPin));
}

TEST_F(AutosarHalTest, AdcReadGroupReturnsConfiguredSample) {
    Adc_ValueGroupType sample = 0u;
    Adc_SetSimulatedGroupSample(kTemperatureGroup, 2345u);

    Adc_StartGroupConversion(kTemperatureGroup);

    EXPECT_EQ(ADC_BUSY, Adc_GetGroupStatus(kTemperatureGroup));
    EXPECT_EQ(E_OK, Adc_ReadGroup(kTemperatureGroup, &sample));
    EXPECT_EQ(2345u, sample);
    EXPECT_EQ(ADC_IDLE, Adc_GetGroupStatus(kTemperatureGroup));
}

TEST_F(AutosarHalTest, HalAdcReadUsesAutosarAdcDriver) {
    Adc_SetSimulatedGroupSample(kTemperatureGroup, 3210u);

    EXPECT_EQ(3210u, hal_adc_read(0u));
}

TEST_F(AutosarHalTest, HalGpioWriteUsesAutosarDioDriver) {
    hal_gpio_write(kAlarmPin, 1u);

    EXPECT_EQ(STD_HIGH, Dio_ReadChannel(kAlarmPin));
    EXPECT_EQ(1u, hal_gpio_read(kAlarmPin));
}
