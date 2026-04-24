# 第2章: テスト容易性の原則とベストプラクティス

## 2.1 悪い例（Before）— テスト困難なコード

組み込み開発でよく見る「テストできないコード」の典型例を示します。

### ❌ Before: bad_temp.c

```c
/* グローバル変数 */
volatile uint16_t g_last_raw_adc = 0;
volatile int16_t g_last_temperature = 0;
volatile uint8_t g_alarm_active = 0;

void bad_read_and_check_temp(void) {
    // ❌ 直接ハードウェアレジスタにアクセス → ホストで実行不可
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) {}
    g_last_raw_adc = ADC1->DR;

    // ❌ ロジックとハードウェアが混在
    int32_t mv = (int32_t)g_last_raw_adc * 3300 / 4095;
    g_last_temperature = (int16_t)(mv / 10);

    // ❌ 閾値チェックと GPIO 制御が一体化
    if (g_last_temperature > 500) {
        GPIOA->ODR |= (1 << 13);   // 直接レジスタアクセス
        g_alarm_active = 1;
    } else {
        GPIOA->ODR &= ~(1 << 13);
        g_alarm_active = 0;
    }
}
```

### この悪いコードの問題点

```mermaid
graph TD
    BAD["bad_read_and_check_temp()"] --> P1["❌ 直接レジスタアクセス\nADC1->CR2, GPIOA->ODR"]
    BAD --> P2["❌ グローバル変数\ng_last_raw_adc 等"]
    BAD --> P3["❌ ロジック混在\n変換 + 判定 + 制御"]
    BAD --> P4["❌ テスト不可\nホストに ADC/GPIO がない"]
    
    P1 --> R1["移植性ゼロ"]
    P2 --> R2["テスト間の干渉"]
    P3 --> R3["バグの切り分け困難"]
    P4 --> R4["実機でしかテストできない"]
    
    style BAD fill:#ffcdd2,stroke:#c62828,stroke-width:2px
    style P1 fill:#ffcdd2
    style P2 fill:#ffcdd2
    style P3 fill:#ffcdd2
    style P4 fill:#ffcdd2
```

## 2.2 良い例（After）— テスト容易な設計

### ✅ After: 3つのレイヤに分離

```mermaid
graph TD
    subgraph After["✅ ベストプラクティス適用後"]
        PURE["純粋関数（テスト容易）\ntemperature_convert()\ntemperature_is_over()\ntemperature_is_valid()"]
        ORCH["オーケストレータ\ntemp_monitor_execute()"]
        HAL_LAYER["HAL関数（副作用を封じ込め）\nhal_adc_read()\nhal_gpio_write()"]
    end
    
    ORCH --> PURE
    ORCH --> HAL_LAYER
    
    style PURE fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style ORCH fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style HAL_LAYER fill:#fff3e0,stroke:#ef6c00,stroke-width:2px
```

### ✅ After: temperature.c（純粋関数）

```c
/* 純粋関数: 入力→出力のみ、副作用なし */
int16_t temperature_convert(uint16_t raw_adc) {
    int32_t mv = (int32_t)raw_adc * 3300 / 4095;
    return (int16_t)(mv / 10);
}

int temperature_is_over(int16_t temp_x10, int16_t threshold_x10) {
    return (temp_x10 > threshold_x10) ? 1 : 0;
}

int temperature_is_valid(uint16_t raw_adc) {
    if (raw_adc == 0 || raw_adc >= 4095) {
        return 0;
    }
    return 1;
}
```

### ✅ After: temp_monitor.c（オーケストレータ）

```c
int16_t temp_monitor_execute(void) {
    uint16_t raw = hal_adc_read(TEMP_ADC_CHANNEL);  /* 副作用 */
    
    if (!temperature_is_valid(raw)) {                 /* 純粋関数 */
        hal_gpio_write(ALARM_LED_PIN, 1);             /* 副作用 */
        return -9999;
    }
    
    int16_t temp = temperature_convert(raw);          /* 純粋関数 */
    int over = temperature_is_over(temp, ALARM_THRESHOLD_X10); /* 純粋関数 */
    hal_gpio_write(ALARM_LED_PIN, (uint8_t)over);     /* 副作用 */
    
    return temp;
}
```

## 2.3 Before vs After 比較

| 項目 | ❌ Before | ✅ After |
|------|-----------|----------|
| ハードウェアアクセス | 直接レジスタ操作 | HAL 関数経由 |
| グローバル変数 | 3つ使用 | なし（引数と戻り値で受け渡し） |
| 関数の責務 | 1関数に全部入り | 変換、判定、制御を分離 |
| テスト方法 | 実機でしかテスト不可 | ホストで全テスト可能 |
| 移植性 | STM32 専用 | HAL 差し替えで任意のMCU対応 |
| テスト数 | 0 | 16 |

## 2.4 純粋関数 vs 副作用 — 詳細

### 純粋関数の特徴

```mermaid
graph LR
    INPUT["入力\n(引数)"] --> FUNC["純粋関数\nf(x) → y"]
    FUNC --> OUTPUT["出力\n(戻り値)"]
    
    NOTE["✅ 同じ入力 → 常に同じ出力\n✅ 外部状態を変更しない\n✅ テストが非常に簡単"]
    
    style FUNC fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style NOTE fill:#f5f5f5,stroke:#999
```

**テスト方法**: 入力を与えて出力を検証するだけ。

```cpp
TEST(TemperatureConvert, ZeroInput) {
    EXPECT_EQ(0, temperature_convert(0));  // これだけ！
}
```

### 副作用を持つ関数の特徴

```mermaid
graph LR
    INPUT2["入力"] --> FUNC2["副作用あり関数"]
    FUNC2 --> OUTPUT2["出力"]
    FUNC2 --> HW["⚠️ ハードウェア変更\nGPIO, ADC..."]
    FUNC2 --> GLOBAL["⚠️ グローバル変数変更"]
    
    NOTE2["⚠️ 実行環境に依存\n⚠️ テストにフェイクが必要\n⚠️ 再現性の確保が困難"]
    
    style FUNC2 fill:#fff3e0,stroke:#ef6c00,stroke-width:2px
    style HW fill:#ffcdd2
    style GLOBAL fill:#ffcdd2
    style NOTE2 fill:#f5f5f5,stroke:#999
```

**テスト方法**: FFF でハードウェア関数をフェイクに差し替え、引数と呼び出し回数を検証。

```cpp
TEST_F(TempMonitorTest, SensorDisconnected_ReturnsError) {
    hal_adc_read_fake.return_val = 0;       // フェイクの戻り値を設定
    int16_t result = temp_monitor_execute(); // テスト実行
    EXPECT_EQ(result, -9999);               // 出力を検証
    EXPECT_EQ(hal_gpio_write_fake.arg1_val, 1);  // 副作用を検証
}
```

## 2.5 移植性の確保

### HAL によるハードウェア抽象化

```mermaid
graph TD
    subgraph App["アプリケーション"]
        MON["temp_monitor.c"]
    end
    subgraph HAL_IF["HAL インターフェース (ヘッダファイル)"]
        ADC_H2["hal_adc.h"]
        GPIO_H2["hal_gpio.h"]
    end
    subgraph Impl["実装を差し替え"]
        STM32["STM32 実装\nhal_adc_stm32.c"]
        NRF["nRF52 実装\nhal_adc_nrf.c"]
        HOST_IMPL["ホスト実装\n(FFF フェイク)"]
    end
    
    MON --> ADC_H2
    MON --> GPIO_H2
    ADC_H2 --> STM32
    ADC_H2 --> NRF
    ADC_H2 --> HOST_IMPL
    GPIO_H2 --> STM32
    GPIO_H2 --> NRF
    GPIO_H2 --> HOST_IMPL
    
    style App fill:#e3f2fd
    style HAL_IF fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style Impl fill:#fff3e0
```

> **ポイント**: アプリケーションコードは HAL ヘッダ（.h）のみに依存。リンク時に実装（.c）を差し替えることで、同じアプリケーションコードが STM32 でも nRF52 でもホストテストでも動作する。

### 移植性を高めるコーディング規約

| 規約 | 理由 | 例 |
|------|------|-----|
| `stdint.h` の型を使う | 型サイズの違いを吸収 | `uint16_t` not `unsigned short` |
| 浮動小数点を避ける | FPU のないMCU対応 | `int16_t temp_x10` (×10表現) |
| 直接レジスタアクセスしない | 移植性確保 | `hal_adc_read()` not `ADC1->DR` |
| グローバル変数を避ける | テスト容易性 | 引数と戻り値で受け渡し |
| `volatile` は HAL 内に限定 | 最適化の影響を限定 | HAL 実装の中だけで使用 |

## 2.6 SOLID 原則（C言語への適用）

### S: 単一責任の原則（SRP）

> 「関数（モジュール）は1つの責任のみを持つべき」

```mermaid
graph LR
    subgraph Bad_SRP["❌ Before: 1関数に全部入り"]
        BAD_F["bad_read_and_check_temp()\n読み取り + 変換 + 判定 + 制御"]
    end
    subgraph Good_SRP["✅ After: 責任を分離"]
        F1["temperature_convert()\n変換のみ"]
        F2["temperature_is_over()\n判定のみ"]
        F3["temperature_is_valid()\n検証のみ"]
        F4["temp_monitor_execute()\n調整のみ"]
    end
    
    style Bad_SRP fill:#ffcdd2
    style Good_SRP fill:#e8f5e9
```

### O: 開放閉鎖の原則（OCP）

> 「拡張に対して開いていて、修正に対して閉じているべき」

HAL のヘッダを変えずに、新しいターゲットの実装ファイルを追加できる。

### D: 依存性逆転の原則（DIP）

> 「上位モジュールは下位モジュールに依存すべきでない。両者は抽象に依存すべき」

```mermaid
graph TD
    subgraph Bad_DIP["❌ Before: 直接依存"]
        APP_BAD["アプリケーション"] --> HW_BAD["ハードウェア\n(STM32レジスタ)"]
    end
    subgraph Good_DIP["✅ After: 抽象に依存"]
        APP_GOOD["アプリケーション"] --> IF_GOOD["HALインターフェース\n(hal_adc.h)"]
        IF_GOOD --> HW_GOOD["HAL実装\n(hal_adc_stm32.c)"]
        IF_GOOD --> FAKE_GOOD["FFF フェイク\n(テスト用)"]
    end
    
    style Bad_DIP fill:#ffcdd2
    style Good_DIP fill:#e8f5e9
    style IF_GOOD fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
```

C言語での DIP の実現方法:

| 方法 | 説明 | 本教材の採用 |
|------|------|-------------|
| ヘッダ + リンカ差し替え | ヘッダで関数宣言、リンク時に実装を選択 | ✅ 採用 |
| 関数ポインタ | 実行時に実装を差し替え | 応用例 |
| コンパイルスイッチ | `#ifdef` でターゲットを切り替え | 補助的に使用 |

## 2.7 リファクタリングの手順

```mermaid
graph TD
    STEP1["1️⃣ 純粋関数を抽出\n変換ロジックを独立関数に"] --> STEP2["2️⃣ HALを定義\nハードウェアアクセスをラップ"]
    STEP2 --> STEP3["3️⃣ グローバル変数を排除\n引数と戻り値に置き換え"]
    STEP3 --> STEP4["4️⃣ テストを書く\n純粋関数 → FFF統合"]
    STEP4 --> STEP5["5️⃣ 全テストを実行\n34テスト全パス"]
    
    style STEP1 fill:#e8f5e9
    style STEP2 fill:#e8f5e9
    style STEP3 fill:#e8f5e9
    style STEP4 fill:#e3f2fd
    style STEP5 fill:#c8e6c9
```

> **ポイント**: リファクタリングは一度に全部やるのではなく、1ステップずつ進めてテストで確認しながら進める。
