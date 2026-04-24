#include "Port.h"

void Port_Init(const Port_ConfigType *ConfigPtr) {
    if (ConfigPtr == NULL_PTR) {
        return;
    }

    Dio_WriteChannel(ConfigPtr->alarm_led_pin, ConfigPtr->alarm_led_initial_level);
}
