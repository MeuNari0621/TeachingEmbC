/**
 * @file temp_alarm_fsm.c
 * @brief 状態遷移・割り込み・イベント駆動の具体例実装
 */
#include "temp_alarm_fsm.h"

#include "temperature.h"

static void temp_alarm_apply_sample(temp_alarm_fsm_t *fsm, uint16_t raw_adc) {
    if (!temperature_is_valid(raw_adc)) {
        fsm->state = TEMP_ALARM_STATE_SENSOR_FAULT;
        fsm->last_temp_x10 = TEMP_ALARM_ERROR_X10;
        fsm->alarm_led_on = 1;
        fsm->sample_requested = 0;
        return;
    }

    fsm->last_temp_x10 = temperature_convert(raw_adc);
    fsm->sample_requested = 0;

    if (temperature_is_over(fsm->last_temp_x10, TEMP_ALARM_THRESHOLD_X10)) {
        fsm->state = TEMP_ALARM_STATE_ALARM;
        fsm->alarm_led_on = 1;
        return;
    }

    fsm->state = TEMP_ALARM_STATE_MONITORING;
    fsm->alarm_led_on = 0;
}

void temp_alarm_fsm_init(temp_alarm_fsm_t *fsm) {
    if (fsm == 0) {
        return;
    }

    fsm->state = TEMP_ALARM_STATE_IDLE;
    fsm->last_temp_x10 = 0;
    fsm->alarm_led_on = 0;
    fsm->sample_requested = 0;
}

void temp_alarm_fsm_dispatch(temp_alarm_fsm_t *fsm, const temp_alarm_event_t *event) {
    if (fsm == 0 || event == 0) {
        return;
    }

    switch (event->type) {
    case TEMP_ALARM_EVENT_START:
        if (fsm->state == TEMP_ALARM_STATE_IDLE) {
            fsm->state = TEMP_ALARM_STATE_MONITORING;
            fsm->alarm_led_on = 0;
            fsm->sample_requested = 1;
        }
        break;

    case TEMP_ALARM_EVENT_STOP:
        temp_alarm_fsm_init(fsm);
        break;

    case TEMP_ALARM_EVENT_TIMER_TICK:
        if (fsm->state != TEMP_ALARM_STATE_IDLE) {
            fsm->sample_requested = 1;
        }
        break;

    case TEMP_ALARM_EVENT_ADC_READY:
        if (fsm->state != TEMP_ALARM_STATE_IDLE) {
            temp_alarm_apply_sample(fsm, event->raw_adc);
        }
        break;

    case TEMP_ALARM_EVENT_ACK_BUTTON:
        if (fsm->state == TEMP_ALARM_STATE_ALARM) {
            fsm->state = TEMP_ALARM_STATE_MONITORING;
            fsm->alarm_led_on = 0;
        }
        break;

    default:
        break;
    }
}

void temp_alarm_fsm_on_timer_interrupt(temp_alarm_fsm_t *fsm) {
    const temp_alarm_event_t event = { TEMP_ALARM_EVENT_TIMER_TICK, 0 };
    temp_alarm_fsm_dispatch(fsm, &event);
}

void temp_alarm_fsm_on_adc_interrupt(temp_alarm_fsm_t *fsm, uint16_t raw_adc) {
    const temp_alarm_event_t event = { TEMP_ALARM_EVENT_ADC_READY, raw_adc };
    temp_alarm_fsm_dispatch(fsm, &event);
}

void temp_alarm_fsm_on_ack_interrupt(temp_alarm_fsm_t *fsm) {
    const temp_alarm_event_t event = { TEMP_ALARM_EVENT_ACK_BUTTON, 0 };
    temp_alarm_fsm_dispatch(fsm, &event);
}