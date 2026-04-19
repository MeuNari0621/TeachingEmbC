/**
 * @file temp_alarm_fsm.c
 * @brief 状態遷移と ISR 分離の具体例実装
 */
#include "temp_alarm_fsm.h"

#include "temperature.h"

static temp_alarm_fsm_t temp_alarm_apply_sample(const temp_alarm_fsm_t *current, uint16_t raw_adc) {
    temp_alarm_fsm_t next = *current;

    if (!temperature_is_valid(raw_adc)) {
        next.state = TEMP_ALARM_STATE_SENSOR_FAULT;
        next.last_temp_x10 = TEMP_ALARM_ERROR_X10;
        next.alarm_led_on = 1;
        next.sample_requested = 0;
        return next;
    }

    next.last_temp_x10 = temperature_convert(raw_adc);
    next.sample_requested = 0;

    if (temperature_is_over(next.last_temp_x10, TEMP_ALARM_THRESHOLD_X10)) {
        next.state = TEMP_ALARM_STATE_ALARM;
        next.alarm_led_on = 1;
        return next;
    }

    next.state = TEMP_ALARM_STATE_MONITORING;
    next.alarm_led_on = 0;
    return next;
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

temp_alarm_fsm_t temp_alarm_transition(const temp_alarm_fsm_t *current, const temp_alarm_event_t *event) {
    temp_alarm_fsm_t next = { 0 };

    if (current == 0 || event == 0) {
        return next;
    }

    next = *current;

    switch (event->type) {
    case TEMP_ALARM_EVENT_START:
        if (next.state == TEMP_ALARM_STATE_IDLE) {
            next.state = TEMP_ALARM_STATE_MONITORING;
            next.alarm_led_on = 0;
            next.sample_requested = 1;
        }
        break;

    case TEMP_ALARM_EVENT_STOP:
        next.state = TEMP_ALARM_STATE_IDLE;
        next.last_temp_x10 = 0;
        next.alarm_led_on = 0;
        next.sample_requested = 0;
        break;

    case TEMP_ALARM_EVENT_TIMER_TICK:
        if (next.state != TEMP_ALARM_STATE_IDLE) {
            next.sample_requested = 1;
        }
        break;

    case TEMP_ALARM_EVENT_ADC_READY:
        if (next.state != TEMP_ALARM_STATE_IDLE) {
            next = temp_alarm_apply_sample(&next, event->raw_adc);
        }
        break;

    case TEMP_ALARM_EVENT_ACK_BUTTON:
        if (next.state == TEMP_ALARM_STATE_ALARM) {
            next.state = TEMP_ALARM_STATE_MONITORING;
            next.alarm_led_on = 0;
        }
        break;

    default:
        break;
    }

    return next;
}

void temp_alarm_fsm_dispatch(temp_alarm_fsm_t *fsm, const temp_alarm_event_t *event) {
    if (fsm == 0 || event == 0) {
        return;
    }

    *fsm = temp_alarm_transition(fsm, event);
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