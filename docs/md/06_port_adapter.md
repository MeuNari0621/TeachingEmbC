# 第6章: ポートアダプタパターン — ハードウェア依存を分離する

## 6.1 ポートアダプタパターンとは

第5章で学んだ依存性逆転の原則（DIP）を、アーキテクチャレベルに拡張したのが**ポートアダプタパターン**（別名: ヘキサゴナルアーキテクチャ）です。DIPが「依存の方向を逆転させる」という原則だったのに対し、ポートアダプタパターンは「アプリケーションの核心ロジックをポート（インターフェース）で囲み、外部をアダプタ（実装）で接続する」という具体的な構造を提供します。

### 基本構造

```mermaid
graph TD
    subgraph 外部 [外部の世界]
        HW[ハードウェア<br>GPIO, UART, SPI...]
        UI[ユーザー<br>インターフェース]
        DB[データ保存<br>EEPROM, Flash...]
    end
    
    subgraph アダプタ層 [アダプタ層]
        A1[GPIOアダプタ]
        A2[UARTアダプタ]
        A3[Flashアダプタ]
    end
    
    subgraph ポート [ポート - インターフェース]
        P1[出力ポート<br>関数宣言ヘッダ]
        P2[入力ポート<br>コールバック定義]
    end
    
    subgraph コア [アプリケーションコア]
        CORE[ビジネスロジック<br>ハードウェア非依存]
    end
    
    HW --- A1
    HW --- A2
    HW --- A3
    A1 --- P1
    A2 --- P1
    A3 --- P1
    P1 --- CORE
    P2 --- CORE
    
    style CORE fill:#e8f5e9,stroke:#2e7d32,stroke-width:3px
    style P1 fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style P2 fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style A1 fill:#fff3e0
    style A2 fill:#fff3e0
    style A3 fill:#fff3e0
    style HW fill:#ffebee
```

### 用語の定義

| 用語 | 説明 | C言語での実現 |
|------|------|-------------|
| **ポート** | アプリケーションコアが外部と通信するためのインターフェース | 関数宣言を含むヘッダファイル（`.h`） |
| **アダプタ** | ポートの具体的な実装 | そのヘッダを実装する `.c` ファイル |
| **アプリケーションコア** | ビジネスロジック本体。外部に依存しない | ポートのヘッダのみをインクルード |

## 6.2 組み込みCでの適用

### 従来の設計（ポートアダプタなし）

```mermaid
graph TD
    subgraph 問題のある設計
        APP[app.c<br>温度監視ロジック]
        APP -->|直接呼び出し| ADC[adc_read 関数<br>ADCレジスタに直接アクセス]
        APP -->|直接呼び出し| UART[uart_send 関数<br>UARTレジスタに直接アクセス]
        APP -->|直接呼び出し| GPIO[gpio_set 関数<br>GPIOレジスタに直接アクセス]
    end
    
    style APP fill:#ffebee
    style ADC fill:#ffcdd2
    style UART fill:#ffcdd2
    style GPIO fill:#ffcdd2
```

この設計では `app.c` がハードウェアに直接依存しており、ホスト環境ではテストできません。

### ポートアダプタ適用後

```mermaid
graph TD
    subgraph アプリケーションコア
        APP[app.c<br>温度監視ロジック]
    end
    
    subgraph ポート - ヘッダファイル
        SENSOR_PORT[sensor_port.h<br>int read_temperature void]
        COMM_PORT[comm_port.h<br>void send_alert const char *msg]
        OUTPUT_PORT[output_port.h<br>void set_alarm bool on]
    end
    
    subgraph 本番アダプタ
        ADC_ADAPTER[adc_adapter.c<br>ADCからの温度読み取り]
        UART_ADAPTER[uart_adapter.c<br>UARTでのアラート送信]
        GPIO_ADAPTER[gpio_adapter.c<br>GPIOでのアラーム制御]
    end
    
    subgraph テスト用アダプタ
        FAKE_SENSOR[fake_sensor.c<br>固定温度値を返す]
        FAKE_COMM[fake_comm.c<br>送信データを記録]
        FAKE_OUTPUT[fake_output.c<br>状態を記録]
    end
    
    APP -->|インクルード| SENSOR_PORT
    APP -->|インクルード| COMM_PORT
    APP -->|インクルード| OUTPUT_PORT
    
    ADC_ADAPTER -.->|実装| SENSOR_PORT
    UART_ADAPTER -.->|実装| COMM_PORT
    GPIO_ADAPTER -.->|実装| OUTPUT_PORT
    
    FAKE_SENSOR -.->|フェイク実装| SENSOR_PORT
    FAKE_COMM -.->|フェイク実装| COMM_PORT
    FAKE_OUTPUT -.->|フェイク実装| OUTPUT_PORT
    
    style APP fill:#e8f5e9,stroke:#2e7d32,stroke-width:3px
    style SENSOR_PORT fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style COMM_PORT fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style OUTPUT_PORT fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style ADC_ADAPTER fill:#fff3e0
    style UART_ADAPTER fill:#fff3e0
    style GPIO_ADAPTER fill:#fff3e0
    style FAKE_SENSOR fill:#f3e5f5
    style FAKE_COMM fill:#f3e5f5
    style FAKE_OUTPUT fill:#f3e5f5
```

## 6.3 本プロジェクトでのポートアダプタ

本プロジェクトの構造を、ポートアダプタの視点で見直してみましょう。

```mermaid
graph TD
    subgraph アプリケーションコア
        ADDC[add.c<br>加算ロジック]
    end
    
    subgraph ポート
        SUBH[sub.h<br>void doubleForFake int *a<br>int sub int a, int b]
    end
    
    subgraph 本番アダプタ
        DRV1[drv1.c<br>doubleForFake の本番実装]
    end
    
    subgraph テスト用アダプタ - FFF
        FAKE[FAKE_VOID_FUNC<br>doubleForFake, int*<br>FFF が自動生成]
    end
    
    ADDC -->|インクルード| SUBH
    DRV1 -.->|実装| SUBH
    FAKE -.->|フェイク実装| SUBH
    
    style ADDC fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style SUBH fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style DRV1 fill:#fff3e0
    style FAKE fill:#f3e5f5
```

| 要素 | ポートアダプタでの役割 | 本プロジェクトでの対応 |
|------|---------------------|---------------------|
| アプリケーションコア | ビジネスロジック | `add.c`（加算処理） |
| ポート | インターフェース定義 | `sub.h`（関数宣言） |
| 本番アダプタ | 実際のハードウェア実装 | `drv1.c`（ドライバ実装） |
| テスト用アダプタ | テスト用のフェイク実装 | FFFマクロによるフェイク |

## 6.4 ポートアダプタ設計のステップ

新しい機能を追加する際の、ポートアダプタを意識した設計手順は以下の通りです。

```mermaid
graph TD
    S1[1. ビジネスロジックを特定する<br>何を計算・判断するか] --> S2[2. 外部依存を特定する<br>何がハードウェアに触るか]
    S2 --> S3[3. ポートを定義する<br>ヘッダファイルに関数宣言を書く]
    S3 --> S4[4. テスト用アダプタを作る<br>FFFでフェイクを定義]
    S4 --> S5[5. テストを書く<br>ビジネスロジックをテスト]
    S5 --> S6[6. ビジネスロジックを実装する<br>テストが通るコードを書く]
    S6 --> S7[7. 本番アダプタを実装する<br>ハードウェアアクセスの実装]
    
    style S1 fill:#e3f2fd
    style S2 fill:#e3f2fd
    style S3 fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style S4 fill:#f3e5f5
    style S5 fill:#e8f5e9
    style S6 fill:#e8f5e9
    style S7 fill:#fff3e0
```

**ポイント**: 本番アダプタ（ハードウェア実装）は**最後に**書きます。ビジネスロジックのテストは、ハードウェアなしでも完了できます。

## 6.5 DIPとポートアダプタの関係

DIP（依存性逆転の原則）とポートアダプタパターンは、同じ問題を異なる抽象度で表現しています。

```mermaid
graph TD
    subgraph 原則レベル
        DIP[DIP<br>依存性逆転の原則<br>上位は下位に依存しない<br>両者は抽象に依存する]
    end
    
    subgraph パターンレベル
        PA[ポートアダプタパターン<br>コアは外部に依存しない<br>ポートを介して通信する]
    end
    
    subgraph 実装レベル
        IMPL[C言語での実現<br>ヘッダ = ポート<br>.c ファイル = アダプタ<br>FFF = テスト用アダプタ]
    end
    
    DIP -->|具体化| PA
    PA -->|具体化| IMPL
    
    style DIP fill:#e3f2fd
    style PA fill:#e8f5e9
    style IMPL fill:#fff3e0
```

| レベル | 概念 | 説明 |
|--------|------|------|
| 原則 | DIP | 依存の方向を逆転させる。上位が下位に依存しない |
| パターン | ポートアダプタ | DIPを構造化したアーキテクチャパターン |
| 実装 | ヘッダ + FFF | C言語での具体的な実現方法 |

## 6.6 AI駆動開発でのポートアダプタ

AIにコード生成を依頼する際、ポートアダプタを意識することで品質が向上します。

### AIへの良い依頼方法

```mermaid
graph TD
    A[1. まずポートを定義<br>ヘッダファイルを自分で書く] --> B[2. テストをAIに依頼<br>このポートに対するテストを書いて]
    B --> C[3. 実装をAIに依頼<br>このテストが通る実装を書いて]
    C --> D[4. テストで検証<br>AIの出力をテストで確認]
    
    style A fill:#fff8e1,stroke:#f9a825,stroke-width:2px
    style B fill:#e8f5e9
    style C fill:#e8f5e9
    style D fill:#e3f2fd
```

**人間が担うべき責任**:
1. **ポート（インターフェース）の定義** — これはアーキテクチャの判断であり、人間が行う
2. **テスト設計の方針決定** — 何をテストするかは人間が決める
3. **生成コードのレビュー** — AIの出力がポートの契約に従っているか確認する

**AIに委ねてよいこと**:
1. テストコードのボイラープレート（定型文）
2. アダプタの実装（ヘッダの関数宣言に合わせた `.c` ファイル）
3. FFFのフェイク定義

```
[AIへの依頼例]
以下のヘッダファイル（ポート）に対して：
- FFFを使ったフェイクの定義
- Google Testのフィクスチャ
- 正常系・異常系のテストケース
を作成してください。

ヘッダファイル:
// sensor_port.h
int read_temperature(void);
bool is_sensor_ready(void);
```
