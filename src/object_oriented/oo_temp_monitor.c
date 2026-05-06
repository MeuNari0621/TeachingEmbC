/**
 * @file oo_temp_monitor.c
 * @brief Cでのオブジェクト指向風温度監視サンプルの実装
 */
#include "oo_temp_monitor.h"

#include "temperature.h"

static int oo_temp_monitor_is_ready(const oo_temp_monitor_t *self) {
    return (self != 0) &&
           (self->adc.read != 0) &&
           (self->led.write != 0);
}

void oo_temp_monitor_init(oo_temp_monitor_t *self,
                          oo_adc_port_t adc,
                          oo_gpio_port_t led,
                          int16_t threshold_x10) {
    if (self == 0) {
        return;
    }

    self->adc = adc;
    self->led = led;
    self->threshold_x10 = threshold_x10;
    self->last_temp_x10 = OO_TEMP_MONITOR_SENSOR_ERROR;
    self->alarm_active = 0;
}

int16_t oo_temp_monitor_run(oo_temp_monitor_t *self) {
    uint16_t raw;
    int16_t temp_x10;

    if (!oo_temp_monitor_is_ready(self)) {
        return OO_TEMP_MONITOR_SENSOR_ERROR;
    }

    raw = self->adc.read(self->adc.context);
    if (!temperature_is_valid(raw)) {
        self->alarm_active = 1;
        self->last_temp_x10 = OO_TEMP_MONITOR_SENSOR_ERROR;
        self->led.write(self->led.context, 1);
        return self->last_temp_x10;
    }

    temp_x10 = temperature_convert(raw);
    self->alarm_active =
        (uint8_t)temperature_is_over(temp_x10, self->threshold_x10);
    self->last_temp_x10 = temp_x10;
    self->led.write(self->led.context, self->alarm_active);

    return self->last_temp_x10;
}

uint8_t oo_temp_monitor_alarm_active(const oo_temp_monitor_t *self) {
    if (self == 0) {
        return 0;
    }

    return self->alarm_active;
}

int16_t oo_temp_monitor_last_temp(const oo_temp_monitor_t *self) {
    if (self == 0) {
        return OO_TEMP_MONITOR_SENSOR_ERROR;
    }

    return self->last_temp_x10;
}
