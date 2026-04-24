#include "Port.h"

void Port_Init(const Port_ConfigType *ConfigPtr) {
    if (ConfigPtr == 0) {
        return;
    }

    Dio_WriteChannel(ConfigPtr->alarm_led_pin, ConfigPtr->alarm_led_initial_level);
}
