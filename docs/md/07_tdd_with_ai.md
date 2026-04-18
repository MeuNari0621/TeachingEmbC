# 第7章: AI駆動開発とテスト駆動開発の融合

## 7.1 AI駆動開発の現在地

GitHub Copilot、ChatGPT、Claude などの AI ツールは、コード生成を劇的に高速化しました。しかし、AI が生成したコードの品質を保証するのは依然として**人間の責任**です。

### AI駆動開発の落とし穴

```mermaid
graph TD
    A[AIにコード生成を依頼] --> B{テストはあるか？}
    B -->|ない| C[AIの出力を目視確認]
    C --> D[見落とし・誤解のリスク]
    D --> E[バグが本番に混入]
    B -->|ある| F[テストで自動検証]
    F --> G[問題の早期発見]
    G --> H[修正して再テスト]
    
    style C fill:#ffebee
    style D fill:#ffcdd2
    style E fill:#ff8a80
    style F fill:#e8f5e9
    style G fill:#c8e6c9
    style H fill:#a5d6a7
```

## 7.2 なぜTDDがAI駆動開発に不可欠か

### 論理的根拠

以下の論理でTDDの必要性を導きます。

```mermaid
graph TD
    P1[前提1: AIは確率的にコードを生成する<br>必ずしも正しいとは限らない] --> C1[帰結1: AIの出力には検証が必要]
    P2[前提2: 人間の目視レビューには限界がある<br>特に複雑なロジックや境界条件] --> C1
    C1 --> C2[帰結2: 自動化された検証手段が必要<br>= テスト]
    P3[前提3: テストを後から書くと<br>仕様が曖昧なまま検証することになる] --> C3[帰結3: テストを先に書くべき<br>= TDD]
    C2 --> C3
    C3 --> C4[結論: AI駆動開発にはTDDが不可欠]
    
    style P1 fill:#fff8e1
    style P2 fill:#fff8e1
    style P3 fill:#fff8e1
    style C1 fill:#e3f2fd
    style C2 fill:#e3f2fd
    style C3 fill:#e3f2fd
    style C4 fill:#e8f5e9,stroke:#2e7d32,stroke-width:3px
```

### AI駆動 × TDDのワークフロー

```mermaid
graph TD
    S1[1. 要件を整理する<br>人間が行う] --> S2[2. テストを設計する<br>人間が行う]
    S2 --> S3[3. テストコードを書く<br>AIの支援を受けてもよい]
    S3 --> S4[4. テストが失敗することを確認<br>Red]
    S4 --> S5[5. AIにプロダクションコード生成を依頼<br>AIに委ねる]
    S5 --> S6{テストは通るか？}
    S6 -->|Yes| S7[7. リファクタリング<br>人間が判断, AIが支援]
    S6 -->|No| S8[6. AIに修正を依頼 or 手動修正]
    S8 --> S6
    S7 --> S9[8. 次の要件へ]
    
    style S1 fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    style S2 fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    style S3 fill:#e8f5e9
    style S4 fill:#ffcccc
    style S5 fill:#e3f2fd
    style S7 fill:#ccccff
    style S8 fill:#fce4ec
```

## 7.3 人間が担うべき判断

AI駆動開発において、人間が注意すべきポイントを体系的に整理します。

### 判断の階層

```mermaid
graph TD
    subgraph 人間が担う
        L1[アーキテクチャ設計<br>モジュール構成, 依存方向]
        L2[テスト戦略<br>何をどのレベルでテストするか]
        L3[インターフェース設計<br>ポートの定義]
        L4[受け入れ基準<br>何をもって完了とするか]
    end
    
    subgraph AIに委ねてよい
        L5[テストコードの雛形生成]
        L6[プロダクションコードの生成]
        L7[リファクタリングの候補提案]
        L8[ドキュメント生成]
    end
    
    subgraph 人間が最終確認
        L9[生成コードのレビュー]
        L10[テスト結果の解釈]
        L11[アーキテクチャの整合性確認]
    end
    
    style L1 fill:#fff8e1
    style L2 fill:#fff8e1
    style L3 fill:#fff8e1
    style L4 fill:#fff8e1
    style L5 fill:#e3f2fd
    style L6 fill:#e3f2fd
    style L7 fill:#e3f2fd
    style L8 fill:#e3f2fd
    style L9 fill:#fce4ec
    style L10 fill:#fce4ec
    style L11 fill:#fce4ec
```

### チェックリスト: AI生成コードのレビュー

| チェック項目 | 確認内容 | 具体例 |
|------------|---------|--------|
| 境界条件 | 入力の最小・最大値でのテストがあるか | `INT_MAX`, `INT_MIN`, `NULL`, 空配列 |
| エラー処理 | 異常系のハンドリングがあるか | メモリ不足、タイムアウト、不正入力 |
| 副作用 | グローバル変数の意図しない変更がないか | 静的変数の初期化漏れ |
| メモリ安全 | バッファオーバーフロー、未初期化アクセスがないか | 配列範囲チェック |
| 依存方向 | 上位が下位に依存していないか | DIPに違反していないか |
| ホスト/ターゲット差異 | 型サイズ、エンディアンの仮定はないか | `int` のサイズに依存するコード |

## 7.4 実践: TDD × AI のステップバイステップ

温度センサーの閾値判定機能を、TDD × AI で開発する手順を示します。

### ステップ1: 要件の整理（人間）

```
要件: 温度が閾値を超えたらアラームを鳴らす
- 温度は整数値（℃）
- 閾値はコンパイル時定数（50℃）
- アラームは ON/OFF の2状態
```

### ステップ2: ポートの定義（人間）

```c
// sensor_port.h — ポート定義（人間が書く）
#ifndef SENSOR_PORT_H
#define SENSOR_PORT_H

#include <stdbool.h>

int read_temperature(void);
void set_alarm(bool on);

#endif
```

### ステップ3: テストの設計と作成（人間 + AI支援）

```cpp
// test_temp_monitor.cpp
#include "gtest/gtest.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

extern "C" {
#include "sensor_port.h"
#include "temp_monitor.h"
}

FAKE_VALUE_FUNC(int, read_temperature);
FAKE_VOID_FUNC(set_alarm, bool);

class TempMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        read_temperature_fake.call_count = 0;
        set_alarm_fake.call_count = 0;
    }
};

// 閾値以下 → アラームOFF
TEST_F(TempMonitorTest, BelowThreshold_AlarmOff) {
    read_temperature_fake.return_val = 49;
    check_temperature();
    EXPECT_EQ(set_alarm_fake.arg0_history[0], false);
}

// 閾値超え → アラームON
TEST_F(TempMonitorTest, AboveThreshold_AlarmOn) {
    read_temperature_fake.return_val = 51;
    check_temperature();
    EXPECT_EQ(set_alarm_fake.arg0_history[0], true);
}

// ちょうど閾値 → アラームOFF（境界条件）
TEST_F(TempMonitorTest, ExactThreshold_AlarmOff) {
    read_temperature_fake.return_val = 50;
    check_temperature();
    EXPECT_EQ(set_alarm_fake.arg0_history[0], false);
}
```

### ステップ4: テストが失敗することを確認（Red）

この時点では `temp_monitor.h` と `check_temperature()` の実装がないため、コンパイルエラーになります。これが「Red」の状態です。

### ステップ5: AIにコード生成を依頼

```
[AIへの依頼]
以下のテストが通るように、temp_monitor.h と temp_monitor.c を作成してください。
- sensor_port.h のインターフェースを使用すること
- 閾値は #define TEMP_THRESHOLD 50 とすること
[テストコードを添付]
```

### ステップ6: AIが生成したコード（例）

```c
// temp_monitor.h
#ifndef TEMP_MONITOR_H
#define TEMP_MONITOR_H

#define TEMP_THRESHOLD 50

void check_temperature(void);

#endif

// temp_monitor.c
#include "temp_monitor.h"
#include "sensor_port.h"

void check_temperature(void) {
    int temp = read_temperature();
    if (temp > TEMP_THRESHOLD) {
        set_alarm(true);
    } else {
        set_alarm(false);
    }
}
```

### ステップ7: テストで検証（Green）

テストを実行し、すべて通ることを確認します。

### ステップ8: レビューと改善

```mermaid
graph TD
    A[テスト通過] --> B{人間によるレビュー}
    B --> C[境界条件は十分か？]
    B --> D[エラー処理は適切か？]
    B --> E[コーディング規約に準拠しているか？]
    B --> F[DIPに違反していないか？]
    
    C --> G[不足があれば<br>テスト追加]
    D --> G
    E --> H[修正 + リファクタリング]
    F --> H
    
    style A fill:#e8f5e9
    style B fill:#fff8e1
    style G fill:#fce4ec
    style H fill:#e3f2fd
```

## 7.5 テスト駆動とAI駆動の相乗効果

```mermaid
graph LR
    subgraph テスト駆動
        T1[仕様を明確にする]
        T2[フィードバックを高速化する]
        T3[リグレッションを防止する]
    end
    
    subgraph AI駆動
        A1[コード生成を高速化する]
        A2[パターンの適用を支援する]
        A3[反復作業を自動化する]
    end
    
    T1 -->|AIへの明確な指示| A1
    A1 -->|生成コードの検証| T2
    T3 -->|安全なリファクタリング| A2
    A3 -->|テスト追加の省力化| T2
    
    style T1 fill:#e8f5e9
    style T2 fill:#e8f5e9
    style T3 fill:#e8f5e9
    style A1 fill:#e3f2fd
    style A2 fill:#e3f2fd
    style A3 fill:#e3f2fd
```

| テスト駆動の強み | AI駆動の強み | 融合による効果 |
|----------------|-------------|--------------|
| 仕様の明確化 | コードの高速生成 | 明確な仕様 → 正確な生成 |
| 即座のフィードバック | パターン適用の支援 | 生成 → 検証 → 修正の高速ループ |
| リグレッション防止 | 反復作業の自動化 | 安全かつ高速な開発 |

## 7.6 アンチパターン: やってはいけないこと

### 1. テストなしでAI生成コードを採用する

```mermaid
graph TD
    A[AIにコード生成を依頼] --> B[生成されたコードを<br>そのままコミット]
    B --> C[バグが後から発覚]
    C --> D[原因特定に時間がかかる]
    D --> E[AIに修正依頼しても<br>同じ過ちを繰り返す]
    
    style A fill:#ffebee
    style B fill:#ffcdd2
    style C fill:#ef9a9a
    style D fill:#e57373
    style E fill:#ef5350
```

### 2. AIにテスト設計を丸投げする

AIにテスト設計を任せると、以下の問題が起こります。

- **網羅性の欠如**: AIは「よくあるケース」は得意だが、ドメイン固有の境界条件を見落とす
- **仕様の誤解**: AIは仕様を推測するため、意図と異なるテストを生成する
- **テストの形骸化**: テストが通るようにテストを書くだけで、実質的な検証にならない

### 3. DIPを無視してAIにアーキテクチャを決めさせる

```mermaid
graph TD
    A[AIにアーキテクチャを一任] --> B[AIは最短距離のコードを生成]
    B --> C[直接依存が増殖]
    C --> D[テスト不能なコード]
    D --> E[ホスト環境テスト断念]
    
    style A fill:#ffebee
    style B fill:#ffcdd2
    style C fill:#ef9a9a
    style D fill:#e57373
    style E fill:#ef5350
```

## 7.7 まとめ: AI時代の組み込み開発者の心得

```mermaid
graph TD
    M1[設計は人間が行う<br>アーキテクチャとインターフェース] --> M2[テストは人間が設計する<br>何をテストするかを決める]
    M2 --> M3[AIは実装の加速に使う<br>テストの雛形やコード生成]
    M3 --> M4[テストで検証する<br>AIの出力を信用しない]
    M4 --> M5[継続的に改善する<br>リファクタリングとテスト追加]
    
    style M1 fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    style M2 fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    style M3 fill:#e3f2fd
    style M4 fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style M5 fill:#f3e5f5
```

1. **設計は人間が行う**: アーキテクチャ（モジュール構成、依存方向）は人間が決める
2. **テストは人間が設計する**: テストで「何を検証するか」は人間が決める
3. **AIは実装を加速する**: テストコードの雛形生成、プロダクションコードの生成に活用
4. **テストで検証する**: AIの出力は必ずテストで検証する
5. **継続的に改善する**: テスト通過後もリファクタリングとテスト追加を行う
