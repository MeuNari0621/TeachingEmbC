/**
 * @file bad_isr.h
 * @brief ❌ 悪い例: ISR で重い処理を実行するコード
 *
 * 教材用の Before 例です。ビルド対象には入れません。
 */
#ifndef BAD_ISR_H
#define BAD_ISR_H

void TIM2_IRQHandler(void);
void ADC1_IRQHandler(void);
void EXTI0_IRQHandler(void);

#endif /* BAD_ISR_H */