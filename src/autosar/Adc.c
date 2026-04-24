#include "Adc.h"

static Adc_GroupType adc_configured_group;
static Adc_ValueGroupType adc_group_sample;
static Adc_StatusType adc_group_status = ADC_IDLE;

void Adc_Init(const Adc_ConfigType *ConfigPtr) {
    adc_configured_group = 0u;
    adc_group_sample = 0u;
    adc_group_status = ADC_IDLE;

    if (ConfigPtr == NULL_PTR) {
        return;
    }

    adc_configured_group = ConfigPtr->temperature_group;
    adc_group_sample = ConfigPtr->initial_sample;
}

void Adc_StartGroupConversion(Adc_GroupType Group) {
    if (Group != adc_configured_group) {
        return;
    }

    adc_group_status = ADC_BUSY;
}

Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType *DataBufferPtr) {
    if (DataBufferPtr == NULL_PTR || Group != adc_configured_group) {
        return E_NOT_OK;
    }

    *DataBufferPtr = adc_group_sample;
    adc_group_status = ADC_IDLE;
    return E_OK;
}

Adc_StatusType Adc_GetGroupStatus(Adc_GroupType Group) {
    if (Group != adc_configured_group) {
        return ADC_IDLE;
    }

    return adc_group_status;
}

void Adc_SetSimulatedGroupSample(Adc_GroupType Group, Adc_ValueGroupType sample) {
    if (Group != adc_configured_group) {
        return;
    }

    adc_group_sample = sample;
}
