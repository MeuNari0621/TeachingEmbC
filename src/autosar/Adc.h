#ifndef ADC_H
#define ADC_H

#include "Std_Types.h"

typedef uint8_t Adc_GroupType;
typedef uint16_t Adc_ValueGroupType;

typedef enum {
    ADC_IDLE = 0,
    ADC_BUSY
} Adc_StatusType;

typedef struct {
    Adc_GroupType temperature_group;
    Adc_ValueGroupType initial_sample;
} Adc_ConfigType;

void Adc_Init(const Adc_ConfigType *ConfigPtr);
void Adc_StartGroupConversion(Adc_GroupType Group);
Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType *DataBufferPtr);
Adc_StatusType Adc_GetGroupStatus(Adc_GroupType Group);
void Adc_SetSimulatedGroupSample(Adc_GroupType Group, Adc_ValueGroupType sample);

#endif /* ADC_H */
