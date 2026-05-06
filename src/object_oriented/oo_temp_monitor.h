/**
 * @file oo_temp_monitor.h
 * @brief Cでオブジェクト指向風に温度監視を表現するサンプル
 *
 * 抽象ポート（ADC/GPIO）と状態を 1 つの構造体へまとめ、
 * 実装差し替えと複数インスタンスを両立させる。
 */
#ifndef OO_TEMP_MONITOR_H
#define OO_TEMP_MONITOR_H

#include <stdint.h>

#define OO_TEMP_MONITOR_SENSOR_ERROR ((int16_t)-9999)

typedef struct {
    uint16_t (*read)(void *context);
    void *context;
} oo_adc_port_t;

typedef struct {
    void (*write)(void *context, uint8_t state);
    void *context;
} oo_gpio_port_t;

typedef struct {
    oo_adc_port_t adc;
    oo_gpio_port_t led;
    int16_t threshold_x10;
    int16_t last_temp_x10;
    uint8_t alarm_active;
} oo_temp_monitor_t;

void oo_temp_monitor_init(oo_temp_monitor_t *self,
                          oo_adc_port_t adc,
                          oo_gpio_port_t led,
                          int16_t threshold_x10);

int16_t oo_temp_monitor_run(oo_temp_monitor_t *self);

uint8_t oo_temp_monitor_alarm_active(const oo_temp_monitor_t *self);

int16_t oo_temp_monitor_last_temp(const oo_temp_monitor_t *self);

#endif /* OO_TEMP_MONITOR_H */
