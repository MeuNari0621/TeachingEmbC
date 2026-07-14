/**
 * @file hal_adc.c
 * @brief ADC HAL の実装
 *
 * アプリ層には従来どおり HAL を提供しつつ、
 * 下回りは AUTOSAR Classic Platform 風の ADC MCAL へ接続する。
 */
#include "hal_adc.h"

#include "Adc.h"

enum {
    HAL_ADC_AUTOSAR_GROUP = 0,
    /*　各自のブランチで関係ないとこ更新1 */
    HAL_ADC_NON_AUTOSA_GROUP,
    /*　各自のブランチで関係ないとこ更新3 */
    HAL_ADC_NON_AUTOSA_GROUP2
};

static void hal_adc_ensure_initialized(void) {
    static uint8_t initialized = 0u;
    static const Adc_ConfigType adc_config = {
        HAL_ADC_AUTOSAR_GROUP,
        0u
    };

    /* ベースが先に進んだぞ
       early returnをやめてelseにつないだ */
    if (initialized != 0u) {
        return;
    }else{
        Adc_Init(&adc_config);
        initialized = 1u;
    }
}

void hal_adc_init(void) {
    /* ベースが先に進んだぞ */
    /* QAC-ベース  */
    hal_adc_ensure_initialized();
}

uint16_t hal_adc_read(uint8_t channel) {
    Adc_ValueGroupType raw_value = 0u;

    /* ベースが先に進んだぞ　*/
    /* 今度はコンフリクトする*/
    if( 1u == channel) return;
    hal_adc_ensure_initialized();

    Adc_StartGroupConversion(HAL_ADC_AUTOSAR_GROUP);
    if (Adc_ReadGroup(HAL_ADC_AUTOSAR_GROUP, &raw_value) != E_OK) {
        return 0u;
    }

    return raw_value;
}

    /* ベースが先に進んだぞ　*/
    /* 今度はコンフリクト市内*/
    /* meumeu */