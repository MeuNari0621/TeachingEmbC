# 第1章: なぜ組み込みCでTDDが必要か

## 1.1 組み込み開発の課題

組み込みソフトウェア開発には、以下のような特有の課題があります。

### ❌ よくある問題

```
「ターゲットボードが届くまでテストできない」
「ハードウェアに依存するコードが多すぎて、バグの切り分けができない」
「テストが属人的で、毎回手動で動作確認している」
```

### 組み込み開発特有のリスク

```mermaid
graph TD
    A["組み込み開発の課題"] --> B["ハードウェア依存"]
    A --> C["テスト環境の制約"]
    A --> D["品質リスク"]
    B --> B1["レジスタ直接アクセス"]
    B --> B2["ペリフェラル初期化"]
    B --> B3["割り込みハンドラ"]
    C --> C1["実機が必要"]
    C --> C2["再現困難なバグ"]
    C --> C3["テスト自動化が困難"]
    D --> D1["リコール・安全問題"]
    D --> D2["フィールドアップデート困難"]
    D --> D3["長期保守コスト"]
    
    style A fill:#ffcdd2,stroke:#c62828
    style B fill:#fff3e0
    style C fill:#fff3e0
    style D fill:#fff3e0
```

## 1.2 解決策: ホストテスト + TDD

これらの課題を解決するアプローチが **ホスト環境でのTDD（テスト駆動開発）** です。

```mermaid
graph LR
    subgraph Host["🖥️ ホスト環境（PC）"]
        GT["Google Test"]
        FFF["FFF（フェイク生成）"]
        SRC["Cソースコード"]
        GT --> SRC
        FFF --> SRC
    end
    subgraph Target["🔌 ターゲット環境"]
        MCU["マイコン"]
        HW["ハードウェア"]
    end
    SRC -->|"同じソースをビルド"| MCU
    
    style Host fill:#e8f5e9,stroke:#2e7d32
    style Target fill:#e3f2fd,stroke:#1565c0
```

**ポイント**: プロダクションコード（C言語）をホストPC上でコンパイル・テストし、同じコードをターゲットにも使う。ハードウェア依存部分は **HAL（ハードウェア抽象化レイヤ）** で分離し、テスト時はフェイクに差し替える。

## 1.3 純粋関数と副作用

本教材の核心的な概念は **純粋関数** と **副作用** の分離です。

### 純粋関数とは

同じ入力に対して常に同じ出力を返し、外部の状態を変更しない関数。

```c
/* ✅ 純粋関数: テストが非常に簡単 */
int16_t temperature_convert(uint16_t raw_adc) {
    int32_t mv = (int32_t)raw_adc * 3300 / 4095;
    return (int16_t)(mv / 10);
}
```

### 副作用を持つ関数とは

ハードウェアにアクセスしたり、グローバル変数を変更する関数。

```c
/* ⚠️ 副作用あり: HAL経由でハードウェアにアクセス */
void hal_gpio_write(uint8_t pin, uint8_t state);
uint16_t hal_adc_read(uint8_t channel);
```

### 設計方針

```mermaid
graph TD
    subgraph Pure["✅ 純粋関数（テスト容易）"]
        P1["temperature_convert()"]
        P2["temperature_is_over()"]
        P3["temperature_is_valid()"]
    end
    subgraph SideEffect["⚠️ 副作用あり（HALで分離）"]
        S1["hal_adc_read()"]
        S2["hal_gpio_write()"]
    end
    subgraph Orchestrator["🔗 オーケストレータ"]
        O1["temp_monitor_execute()"]
    end
    
    O1 --> S1
    O1 --> P1
    O1 --> P2
    O1 --> P3
    O1 --> S2
    
    style Pure fill:#e8f5e9,stroke:#2e7d32
    style SideEffect fill:#fff3e0,stroke:#ef6c00
    style Orchestrator fill:#e3f2fd,stroke:#1565c0
```

> **原則**: ロジックは純粋関数に、ハードウェアアクセスはHAL関数に分離する。オーケストレータがこれらを組み合わせる。

## 1.4 TDD の 3 ステップ（Red → Green → Refactor）

```mermaid
graph LR
    R["🔴 Red\nまずテストを書く\n（失敗する）"] --> G["🟢 Green\n最小限の実装で\nテストを通す"]
    G --> RF["🔵 Refactor\nコードを\n整理する"]
    RF --> R
    
    style R fill:#ffcdd2,stroke:#c62828
    style G fill:#c8e6c9,stroke:#2e7d32
    style RF fill:#bbdefb,stroke:#1565c0
```

1. **Red**: 実装前にテストを書く → 当然テストは失敗する
2. **Green**: テストが通る最小限のコードを書く
3. **Refactor**: 重複を排除し、設計を改善する（テストは通ったまま）

## 1.5 本教材の学習ゴール

| ゴール | 内容 |
|--------|------|
| テスト容易性 | 純粋関数と副作用を分離し、テストしやすい設計ができる |
| 移植性 | HAL を介してハードウェア依存を分離し、ホスト/ターゲット両方でビルドできる |
| SOLID 原則 | C言語で SOLID を適用し、変更に強い設計ができる |
| AI 活用 | AI にテストとコードを生成させ、人間がレビューする開発フローを実践できる |

## 1.6 本教材で使う題材

温度監視システムを例に学びます。

```mermaid
graph LR
    SENSOR["🌡️ 温度センサ\n（ADC入力）"] --> CONVERT["📐 温度変換\n（純粋関数）"]
    CONVERT --> CHECK["✅ 閾値チェック\n（純粋関数）"]
    CHECK --> LED["💡 アラームLED\n（GPIO出力）"]
    
    style SENSOR fill:#e3f2fd
    style CONVERT fill:#e8f5e9
    style CHECK fill:#e8f5e9
    style LED fill:#fff3e0
```
