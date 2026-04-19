/**
 * @file bad_state_machine.h
 * @brief ❌ 悪い例: 状態遷移と副作用が混在したコード
 *
 * 教材用の Before 例です。ビルド対象には入れません。
 */
#ifndef BAD_STATE_MACHINE_H
#define BAD_STATE_MACHINE_H

#include <stdint.h>

extern volatile uint8_t g_alarm_state;
extern volatile uint8_t g_sample_due;
extern volatile int16_t g_last_temp_x10;

void bad_temp_alarm_task(void);

#endif /* BAD_STATE_MACHINE_H */