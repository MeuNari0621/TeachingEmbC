# 第7章: AI駆動開発とTDD

## 7.1 なぜ AI 時代に TDD が重要か

AI（大規模言語モデル / LLM）がコードを生成できる時代になりました。しかし、AI が生成するコードには以下の特性があります。

```mermaid
graph TD
    AI["🤖 AI（LLM）の特性"] --> G1["✅ 大量のコードを高速に生成"]
    AI --> G2["✅ パターンに基づく実装"]
    AI --> G3["✅ ボイラープレートの自動化"]
    AI --> B1["⚠️ 文脈の取り違え"]
    AI --> B2["⚠️ エッジケースの見落とし"]
    AI --> B3["⚠️ ハルシネーション（もっともらしい嘘）"]
    
    style AI fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style G1 fill:#e8f5e9
    style G2 fill:#e8f5e9
    style G3 fill:#e8f5e9
    style B1 fill:#ffcdd2
    style B2 fill:#ffcdd2
    style B3 fill:#ffcdd2
```

### TDD が AI の弱点を補う論理

```
前提1: AI は仕様を正しく理解しているとは限らない
前提2: テストは「仕様の形式化」である
前提3: テストが先にあれば、AI 生成コードの正しさを自動検証できる
結論: AI 駆動開発では TDD がさらに重要になる
```

```mermaid
graph LR
    SPEC["📋 人間が仕様を定義"] --> TEST_FIRST["✅ テストとして形式化"]
    TEST_FIRST --> AI_GEN["🤖 AI にコード生成を依頼"]
    AI_GEN --> RUN["🏃 テスト実行"]
    RUN -->|"PASS"| REVIEW["👁️ 人間がレビュー"]
    RUN -->|"FAIL"| FIX["🔧 AI に修正を依頼"]
    FIX --> RUN
    REVIEW --> DONE["✅ 完了"]
    
    style SPEC fill:#fff3e0
    style TEST_FIRST fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style AI_GEN fill:#e3f2fd
    style RUN fill:#e8f5e9
```

## 7.2 AI駆動開発のワークフロー

### ステップ1: 仕様をテストとして書く（人間）

```cpp
// 人間がまず「何が正しいか」をテストで定義する
TEST(TemperatureConvert, ZeroInput) {
    EXPECT_EQ(0, temperature_convert(0));
}

TEST(TemperatureConvert, MaxInput) {
    EXPECT_EQ(330, temperature_convert(4095));
}

TEST(TemperatureIsValid, SensorDisconnected) {
    EXPECT_EQ(0, temperature_is_valid(0));
}
```

### ステップ2: AI にコード生成を依頼

```
【AI への指示例】
以下のヘッダファイルの関数を実装してください。
テストファイル test_app.cpp が全て PASS するようにしてください。

条件:
- 純粋関数として実装
- stdint.h の型を使用
- 浮動小数点は使用しない
- ADC は 12bit (0-4095)、基準電圧 3.3V
```

### ステップ3: テスト実行 → レビュー

```mermaid
sequenceDiagram
    participant Human as 👤 人間
    participant AI as 🤖 AI
    participant Test as ✅ テスト
    
    Human->>Human: 仕様をテストとして記述
    Human->>AI: テスト + ヘッダを渡す
    AI->>AI: コード生成
    AI->>Test: 生成コードをビルド
    Test->>Test: テスト実行
    alt 全テスト PASS
        Test->>Human: 結果報告
        Human->>Human: コードレビュー
    else テスト FAIL
        Test->>AI: 失敗結果を渡す
        AI->>AI: 修正
    end
```

## 7.3 人間が気をつけるべきこと

### 人間 vs AI の責任分担

| 責任 | 人間 | AI |
|------|------|-----|
| 仕様の定義 | ✅ 主担当 | 参考意見 |
| テストの設計 | ✅ 主担当（境界値、異常系） | 補助生成 |
| コード生成 | レビュー | ✅ 主担当 |
| テスト実行 | 結果確認 | ✅ 自動実行 |
| アーキテクチャ決定 | ✅ 主担当 | 提案 |
| セキュリティ確認 | ✅ 最終確認 | スキャン |

### 人間が確認すべきチェックリスト

```mermaid
graph TD
    CHECK["人間が確認すべきポイント"] --> C1["✅ テストが仕様を正しく表現しているか"]
    CHECK --> C2["✅ 境界値テストがあるか"]
    CHECK --> C3["✅ 異常系テストがあるか"]
    CHECK --> C4["✅ 副作用が HAL に封じ込められているか"]
    CHECK --> C5["✅ グローバル変数を使っていないか"]
    CHECK --> C6["✅ stdint.h の型を使っているか"]
    CHECK --> C7["✅ 浮動小数点を避けているか"]
    
    style CHECK fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style C1 fill:#e8f5e9
    style C2 fill:#e8f5e9
    style C3 fill:#e8f5e9
    style C4 fill:#e8f5e9
    style C5 fill:#e8f5e9
    style C6 fill:#e8f5e9
    style C7 fill:#e8f5e9
```

## 7.4 AI に出す指示のベストプラクティス

### 良い指示の例

```
✅ 「temperature_convert 関数を実装してください。
    - 入力: uint16_t raw_adc (0-4095)
    - 出力: int16_t (温度×10)
    - 変換式: mv = raw * 3300 / 4095, temp_x10 = mv / 10
    - テストファイル test_app.cpp が全 PASS すること」
```

### 悪い指示の例

```
❌ 「温度変換する関数を作って」
→ 仕様が曖昧で、AI が勝手に浮動小数点を使ったり、
  型が合わなかったりする
```

### AI 指示テンプレート

```
【関数実装の依頼】
■ 対象ヘッダ: (ファイルパス)
■ 関数名: (関数名)
■ 制約条件:
  - 純粋関数 / HAL関数のどちらか
  - 使用する型（stdint.h）
  - 浮動小数点の使用可否
  - 対応するテストファイル
■ テスト実行コマンド:
  cd build && cmake --build . && ctest --output-on-failure
```

## 7.5 アンチパターン

### ❌ テストなしで AI にコード生成させる

```mermaid
graph LR
    AI_BAD["🤖 AI がコード生成"] --> NO_TEST["❌ テストなし"]
    NO_TEST --> BUG["🐛 バグ潜伏"]
    BUG --> FIELD["💥 実機で発覚"]
    
    style AI_BAD fill:#e3f2fd
    style NO_TEST fill:#ffcdd2
    style BUG fill:#ffcdd2
    style FIELD fill:#ffcdd2
```

### ❌ AI 生成テストを鵜呑みにする

```mermaid
graph LR
    AI_TEST["🤖 AI がテスト生成"] --> TRUST["❌ 人間がレビューしない"]
    TRUST --> MISS["仕様を反映していない\nテストが通るだけのコード"]
    
    style AI_TEST fill:#e3f2fd
    style TRUST fill:#ffcdd2
    style MISS fill:#ffcdd2
```

### ✅ 正しいフロー

```mermaid
graph LR
    HUMAN_SPEC["👤 人間が仕様定義"] --> HUMAN_TEST["👤 テスト設計"]
    HUMAN_TEST --> AI_IMPL["🤖 AI が実装"]
    AI_IMPL --> AUTO_TEST["✅ 自動テスト"]
    AUTO_TEST --> HUMAN_REVIEW["👤 人間がレビュー"]
    
    style HUMAN_SPEC fill:#fff3e0
    style HUMAN_TEST fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style AI_IMPL fill:#e3f2fd
    style AUTO_TEST fill:#e8f5e9
    style HUMAN_REVIEW fill:#fff3e0
```

## 7.6 まとめ: AI 時代の開発フロー

```mermaid
graph TD
    START["開始"] --> S1["1️⃣ 仕様を明確化\n（人間）"]
    S1 --> S2["2️⃣ テストを先に書く\n（人間 + AI補助）"]
    S2 --> S3["3️⃣ HAL/純粋関数の設計\n（人間）"]
    S3 --> S4["4️⃣ AI に実装を依頼"]
    S4 --> S5["5️⃣ テスト実行\n（自動）"]
    S5 -->|"FAIL"| S6["6️⃣ AI に修正依頼"]
    S6 --> S5
    S5 -->|"PASS"| S7["7️⃣ 人間がレビュー"]
    S7 -->|"問題あり"| S4
    S7 -->|"OK"| S8["8️⃣ リファクタリング"]
    S8 --> S5
    
    style S1 fill:#fff3e0
    style S2 fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style S3 fill:#fff3e0
    style S4 fill:#e3f2fd
    style S5 fill:#e8f5e9
    style S7 fill:#fff3e0
    style S8 fill:#bbdefb
```
