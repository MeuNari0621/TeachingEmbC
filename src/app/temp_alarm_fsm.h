/**
 * @file temp_alarm_fsm.h
 * @brief 状態遷移と ISR 分離の具体例
 *
 * 状態遷移そのものは純粋関数として切り出し、
 * ISR はイベントを発行する薄いラッパーに留める。
 */
#ifndef TEMP_ALARM_FSM_H
#define TEMP_ALARM_FSM_H

#include <stdint.h>

/** アラーム閾値（×10, 例: 300 = 30.0℃） */
#define TEMP_ALARM_THRESHOLD_X10 300
/** センサ異常時のエラー値 */
#define TEMP_ALARM_ERROR_X10 (-9999)

/** 状態遷移の一覧 */
typedef enum {
    TEMP_ALARM_STATE_IDLE = 0,
    TEMP_ALARM_STATE_MONITORING,
    TEMP_ALARM_STATE_ALARM,
    TEMP_ALARM_STATE_SENSOR_FAULT
} temp_alarm_state_t;

/** イベント駆動で受け取るイベント種別 */
typedef enum {
    TEMP_ALARM_EVENT_START = 0,
    TEMP_ALARM_EVENT_STOP,
    TEMP_ALARM_EVENT_TIMER_TICK,
    TEMP_ALARM_EVENT_ADC_READY,
    TEMP_ALARM_EVENT_ACK_BUTTON
} temp_alarm_event_type_t;

/** 状態機械へ投入するイベント */
typedef struct {
    temp_alarm_event_type_t type;
    uint16_t raw_adc;
} temp_alarm_event_t;

/** 状態機械の保持状態 */
typedef struct {
    temp_alarm_state_t state;
    int16_t last_temp_x10;
    uint8_t alarm_led_on;
    uint8_t sample_requested;
} temp_alarm_fsm_t;

void temp_alarm_fsm_init(temp_alarm_fsm_t *fsm);
temp_alarm_fsm_t temp_alarm_transition(const temp_alarm_fsm_t *current, const temp_alarm_event_t *event);
void temp_alarm_fsm_dispatch(temp_alarm_fsm_t *fsm, const temp_alarm_event_t *event);

/**
 * @brief タイマ割り込みの具体例
 *
 * ISR 本体では重い処理をせず、「サンプリング要求イベント」を上げるだけにする。
 */
void temp_alarm_fsm_on_timer_interrupt(temp_alarm_fsm_t *fsm);

/**
 * @brief ADC 変換完了割り込みの具体例
 *
 * 取得済みの ADC 生値をイベントとして状態機械へ渡す。
 */
void temp_alarm_fsm_on_adc_interrupt(temp_alarm_fsm_t *fsm, uint16_t raw_adc);

/**
 * @brief ボタン割り込みの具体例
 *
 * ユーザが ACK ボタンを押したことをイベントで渡す。
 */
void temp_alarm_fsm_on_ack_interrupt(temp_alarm_fsm_t *fsm);

#endif /* TEMP_ALARM_FSM_H */