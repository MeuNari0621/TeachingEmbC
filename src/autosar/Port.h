#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"
#include "Dio.h"

typedef uint8_t Port_PinType;

typedef struct {
    Port_PinType alarm_led_pin;
    Dio_LevelType alarm_led_initial_level;
} Port_ConfigType;

void Port_Init(const Port_ConfigType *ConfigPtr);

#endif /* PORT_H */
