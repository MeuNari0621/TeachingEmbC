# 第12章: AUTOSAR準拠の下回りへ寄せる

## 12.1 この教材で参照した AUTOSAR REV

本教材の下回り改造では **AUTOSAR Classic Platform R24-11** を基準にしました。参照元は公式サイトと公式配布資料です。

- Classic Platform 公式ページ: https://www.autosar.org/standards/classic-platform
- Foundation Release Overview R24-11: https://www.autosar.org/fileadmin/standards/R24-11/FO/AUTOSAR_FO_TR_ReleaseOverview.pdf
- AUTOSAR Introduction Part 1: https://www.autosar.org/fileadmin/user_upload/AUTOSAR_EXP_Introduction_Part1.pdf
- AUTOSAR Introduction Part 2: https://www.autosar.org/fileadmin/user_upload/AUTOSAR_EXP_Introduction_Part2.pdf
- Safety Overview: https://www.autosar.org/fileadmin/standards/R25-11/FO/AUTOSAR_FO_EXP_SafetyOverview.pdf

> 注意: 本リポジトリは教材なので、完全な BSW / RTE / ECUC 一式を実装したわけではありません。**ADC/DIO/Port の責務分離、命名、層構造を AUTOSAR Classic Platform に合わせる方向で簡略準拠**しています。

## 12.2 AUTOSAR 入門

AUTOSAR は車載ソフトウェアの再利用性、移植性、品質確保を狙う標準です。Classic Platform では、下回りを段階的に分けて責務を固定します。

| 層 | 役割 | 本教材での対応 |
|----|------|----------------|
| Application | 業務ロジック | `temperature.c`, `temp_monitor.c`, `temp_alarm_fsm.c` |
| RTE | SWC 間の接続 | 本教材では未導入 |
| Basic Software | 共通サービス群 | 本教材では最小限 |
| MCAL | MCU 依存ドライバ | `src/autosar/Adc.*`, `Dio.*`, `Port.*` |

今回の改造では、従来 `hal_adc.c` / `hal_gpio.c` が直接ハード依存スタブを持っていた部分を、**HAL → AUTOSAR風MCAL** の二段構成へ変更しました。

## 12.3 どこを AUTOSAR 風にしたか

| 変更前 | 変更後 | 意図 |
|--------|--------|------|
| `hal_adc_read()` が単独スタブ | `hal_adc_read()` → `Adc_StartGroupConversion()` / `Adc_ReadGroup()` | ADC責務を MCAL に寄せる |
| `hal_gpio_write()` が単独スタブ | `hal_gpio_write()` → `Port_Init()` / `Dio_WriteChannel()` | I/O責務を Port/Dio に分ける |
| 共通標準型なし | `Std_Types.h` を追加 | AUTOSAR系の型・戻り値を合わせる |

これにより、アプリ層は従来どおり `hal_*` を見ればよく、下回りだけを AUTOSAR 風へ交換できます。教材としては、**上位のテスト容易性を壊さず、下位の標準化を進める**形です。

## 12.4 品質保証で意識すること

AUTOSAR は単なる API 名の置換ではなく、品質保証のための分業と検証容易性が重要です。

- **責務分離**: ADC, DIO, Port を分けて、変更影響を局所化する
- **インターフェース固定**: 上位は標準化された境界に依存する
- **安全規格との整合**: 実案件では ISO 26262 と合わせて要求、故障検出、診断を整理する
- **静的品質**: MISRA C、静的解析、コードレビュー、構成管理を前提にする
- **診断設計**: 実案件では DET/DEM で異常通知経路を定義する

本教材では DET/DEM や RTE までは踏み込みませんが、**層分離とテスト戦略を先に整える**ことで AUTOSAR 的な品質保証の入口を作っています。

## 12.5 テスト戦略

AUTOSAR 準拠化で大事なのは、下回りを増やしてもテストしやすさを失わないことです。

| レベル | 対象 | 方法 |
|--------|------|------|
| 純粋関数テスト | `temperature.c`, `temp_alarm_transition()` | Google Test |
| 境界テスト | `temp_monitor.c` | FFF で `hal_*` をフェイク化 |
| MCALテスト | `Adc.*`, `Dio.*`, `Port.*`, `hal_*` アダプタ | `test_autosar_hal.cpp` |
| 実機結合 | 実MCU依存ドライバ | ターゲット確認 |

今回の追加テストでは、次をホストで確認します。

- `Dio_WriteChannel()` と `Dio_ReadChannel()` の整合
- `Adc_StartGroupConversion()` と `Adc_ReadGroup()` の整合
- 既存 HAL が AUTOSAR 風 MCAL を正しく呼ぶこと
- 既存アプリ層と追加 MCAL テストを含む 34 テストが壊れないこと

## 12.6 まとめ

この教材での AUTOSAR 対応は、**下回りを AUTOSAR Classic Platform R24-11 の考え方に寄せ、上位の TDD しやすさを維持する**ことを狙っています。最初からフルスタックの AUTOSAR を入れるのではなく、まずは **MCAL 相当の責務分離、標準型、テスト境界** から始めると学習と実装の両方が進めやすくなります。
