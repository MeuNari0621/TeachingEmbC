# 第6章: ポートアダプタパターン

## 6.1 ポートアダプタとは

ポートアダプタパターン（別名: ヘキサゴナルアーキテクチャ）は、アプリケーションのコアロジックを外部依存から分離する設計パターンです。

```mermaid
graph TD
    subgraph Core["🎯 アプリケーションコア"]
        LOGIC["ビジネスロジック\n（純粋関数）"]
    end
    subgraph Ports["🔌 ポート（インターフェース）"]
        PORT_IN["入力ポート\n（ADC読み取り）"]
        PORT_OUT["出力ポート\n（GPIO制御）"]
    end
    subgraph Adapters["🔧 アダプタ（実装）"]
        ADP_STM["STM32アダプタ"]
        ADP_NRF["nRF52アダプタ"]
        ADP_TEST["テスト用フェイク"]
    end
    
    PORT_IN --> LOGIC
    LOGIC --> PORT_OUT
    ADP_STM --> PORT_IN
    ADP_NRF --> PORT_IN
    ADP_TEST --> PORT_IN
    PORT_OUT --> ADP_STM
    PORT_OUT --> ADP_NRF
    PORT_OUT --> ADP_TEST
    
    style Core fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style Ports fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style Adapters fill:#fff3e0,stroke:#ef6c00
```

## 6.2 本教材での実現

DIP（第5章）で学んだ原則を、アーキテクチャレベルで適用したものがポートアダプタです。

### C言語での対応表

| ポートアダプタの概念 | 本教材での実装 | ファイル |
|--------------------|---------------|---------|
| アプリケーションコア | 純粋関数群 | `temperature.c` |
| オーケストレータ | 統合ロジック | `temp_monitor.c` |
| ポート | HAL ヘッダファイル | `hal_adc.h`, `hal_gpio.h` |
| 実機アダプタ | HAL 実装 | `hal_adc.c`, `hal_gpio.c` |
| テストアダプタ | FFF フェイク | `test_drv.cpp` 内 |

### プロジェクト構造との対応

```mermaid
graph TD
    subgraph CoreFiles["アプリケーションコア"]
        TEMP_C["temperature.c\n（純粋関数）"]
        MON_C["temp_monitor.c\n（オーケストレータ）"]
    end
    subgraph PortFiles["ポート（インターフェース）"]
        ADC_H3["hal_adc.h"]
        GPIO_H3["hal_gpio.h"]
    end
    subgraph AdapterFiles["アダプタ（実装）"]
        ADC_C2["hal_adc.c\n（実機用）"]
        GPIO_C2["hal_gpio.c\n（実機用）"]
        TEST_FFF["test_drv.cpp 内の\nFAKE_VALUE_FUNC\n（テスト用）"]
    end
    
    MON_C --> TEMP_C
    MON_C --> ADC_H3
    MON_C --> GPIO_H3
    ADC_H3 -.-> ADC_C2
    ADC_H3 -.-> TEST_FFF
    GPIO_H3 -.-> GPIO_C2
    GPIO_H3 -.-> TEST_FFF
    
    style CoreFiles fill:#e8f5e9,stroke:#2e7d32
    style PortFiles fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style AdapterFiles fill:#fff3e0,stroke:#ef6c00
```

## 6.3 DIP との関係

ポートアダプタパターンは DIP をアーキテクチャ全体に適用したものです。

```mermaid
graph LR
    DIP["DIP\n（設計原則）"] -->|"アーキテクチャに適用"| PA["ポートアダプタ\n（設計パターン）"]
    PA -->|"C言語で実現"| HAL_PA["HAL + リンカ差し替え"]
    
    style DIP fill:#e3f2fd
    style PA fill:#e8f5e9
    style HAL_PA fill:#fff3e0
```

| レベル | 概念 | 実装 |
|--------|------|------|
| 原則 | DIP（依存性逆転） | 上位→抽象←下位 |
| パターン | ポートアダプタ | コア＋ポート＋アダプタ |
| 実装 | HAL + リンカ差し替え | ヘッダ(.h) + 実装(.c) 分離 |

## 6.4 新しいターゲットへの移植手順

```mermaid
graph TD
    STEP1_PA["1️⃣ HAL ヘッダを確認\nhal_adc.h の関数宣言を確認"] --> STEP2_PA["2️⃣ 新しいアダプタを作成\nhal_adc_nrf.c を新規作成"]
    STEP2_PA --> STEP3_PA["3️⃣ CMake を修正\n新アダプタをリンク対象に"]
    STEP3_PA --> STEP4_PA["4️⃣ ホストテストは変更なし\nFFF フェイクはそのまま"]
    STEP4_PA --> STEP5_PA["5️⃣ ターゲットでビルド\narm-none-eabi-gcc でコンパイル"]
    
    style STEP1_PA fill:#e3f2fd
    style STEP2_PA fill:#e8f5e9
    style STEP3_PA fill:#fff3e0
    style STEP4_PA fill:#e8f5e9
    style STEP5_PA fill:#e3f2fd
```

> **ポイント**: アプリケーションコード（`temperature.c`, `temp_monitor.c`）は一切変更不要。HAL の実装ファイルだけを追加すれば、新しいハードウェアに対応できる。

## 6.5 テスト戦略

```mermaid
graph TD
    subgraph TestStrategy["テスト戦略"]
        L1["レベル1: 純粋関数テスト\n（フェイク不要）"]
        L2["レベル2: FFF統合テスト\n（HALをフェイク化）"]
        L3["レベル3: ターゲットテスト\n（実機で結合テスト）"]
    end
    
    L1 -->|"100%ホストで実行"| HOST1["test_temperature\n11テスト"]
    L2 -->|"100%ホストで実行"| HOST2["test_temp_monitor\n5テスト"]
    L3 -->|"実機で実行"| TARGET["手動テスト\nJTAGデバッグ"]
    
    style L1 fill:#e8f5e9
    style L2 fill:#e3f2fd
    style L3 fill:#fff3e0
```

| レベル | 対象 | 方法 | カバー範囲 |
|--------|------|------|-----------|
| 純粋関数テスト | `temperature.c` | Google Test のみ | ロジック100% |
| FFF統合テスト | `temp_monitor.c` | Google Test + FFF | 統合フロー |
| ターゲットテスト | HAL 実装 | 実機 + JTAG | ハードウェア接続 |

> **目標**: レベル1 と レベル2 でバグの90%以上を検出し、ターゲットテストは最小限にする。
