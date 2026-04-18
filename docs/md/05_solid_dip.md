# 第5章: SOLID原則と依存性逆転 — テストしやすい設計とは

## 5.1 SOLID原則の概要

SOLID原則は、ソフトウェア設計の5つの基本原則です。組み込みC開発においても、テストしやすく保守しやすいコードを書くために重要です。

```mermaid
graph TD
    SOLID[SOLID原則] --> S[S: 単一責任の原則<br>Single Responsibility]
    SOLID --> O[O: 開放閉鎖の原則<br>Open-Closed]
    SOLID --> L[L: リスコフの置換原則<br>Liskov Substitution]
    SOLID --> I[I: インターフェース分離の原則<br>Interface Segregation]
    SOLID --> D[D: 依存性逆転の原則<br>Dependency Inversion]
    
    style SOLID fill:#e3f2fd
    style S fill:#e8f5e9
    style O fill:#fff8e1
    style L fill:#fce4ec
    style I fill:#f3e5f5
    style D fill:#ffccbc,stroke:#ff5722,stroke-width:3px
```

本教材では特に**D（依存性逆転の原則：DIP）**に注目しますが、まず各原則を組み込みCの文脈で概観します。

## 5.2 各原則の組み込みCでの適用

### S: 単一責任の原則（SRP）

> 「モジュールは、変更の理由を1つだけ持つべきである」

```mermaid
graph LR
    subgraph 悪い例
        BAD[sensor_manager.c<br>センサー読み取り<br>+ データ変換<br>+ UART送信<br>+ エラーログ]
    end
    subgraph 良い例
        G1[sensor_read.c<br>センサー読み取り]
        G2[data_convert.c<br>データ変換]
        G3[uart_send.c<br>UART送信]
        G4[error_log.c<br>エラーログ]
    end
    
    style BAD fill:#ffebee
    style G1 fill:#e8f5e9
    style G2 fill:#e8f5e9
    style G3 fill:#e8f5e9
    style G4 fill:#e8f5e9
```

組み込みCでは、1つの `.c` ファイルに多くの機能を詰め込みがちです。機能ごとにファイルを分割することで、テストが書きやすくなります。

### O: 開放閉鎖の原則（OCP）

> 「拡張に対して開いており、修正に対して閉じているべきである」

```mermaid
graph TD
    subgraph 関数ポインタで拡張
        IF[インターフェース<br>関数ポインタテーブル]
        IF --> IMPL1[UART実装]
        IF --> IMPL2[SPI実装]
        IF --> IMPL3[将来の追加も<br>既存コード変更不要]
    end
    
    style IF fill:#e3f2fd
    style IMPL1 fill:#e8f5e9
    style IMPL2 fill:#e8f5e9
    style IMPL3 fill:#fff8e1
```

C言語ではインターフェースや継承が言語機能として存在しませんが、**関数ポインタ**を使うことで同様の柔軟性を実現できます。

### L: リスコフの置換原則（LSP）

> 「基底型のオブジェクトを派生型のオブジェクトで置き換えても、プログラムの正しさが保たれるべきである」

C言語ではクラス階層がありませんが、関数ポインタのテーブル（≒インターフェース）を「基底型」と見なせば、どの実装に差し替えても同じ契約を満たすべきという原則が適用できます。

### I: インターフェース分離の原則（ISP）

> 「クライアントが使わないメソッドに依存させてはならない」

```mermaid
graph TD
    subgraph 悪い例
        FAT[巨大ヘッダ<br>hal.h<br>GPIO + UART + SPI + I2C + ADC]
        C1[gpio_app.c] --> FAT
        C2[uart_app.c] --> FAT
    end
    subgraph 良い例
        H1[hal_gpio.h]
        H2[hal_uart.h]
        H3[hal_spi.h]
        C3[gpio_app.c] --> H1
        C4[uart_app.c] --> H2
    end
    
    style FAT fill:#ffebee
    style H1 fill:#e8f5e9
    style H2 fill:#e8f5e9
    style H3 fill:#e8f5e9
```

組み込みCでは、HAL（Hardware Abstraction Layer）のヘッダを機能ごとに分割することが、この原則に相当します。

### D: 依存性逆転の原則（DIP）

> 「上位モジュールは下位モジュールに依存すべきではない。両者は抽象に依存すべきである」

この原則は、テスト可能な組み込みコード設計の**核心**です。

## 5.3 依存性逆転の原則（DIP）を深掘りする

### DIPが解決する問題

従来の組み込み設計では、アプリケーション層がドライバ層に直接依存しています。

```mermaid
graph TD
    subgraph DIPなし - 従来の設計
        APP1[APP層<br>add.c] -->|直接依存| DRV1[DRV層<br>drv1.c]
        DRV1 -->|直接アクセス| HW1[ハードウェア]
    end
    
    style APP1 fill:#e8f5e9
    style DRV1 fill:#ffebee
    style HW1 fill:#ffcdd2
```

この構造では、APP層をテストするにはDRV層（さらにはハードウェア）が必要になります。

### DIPの適用後

```mermaid
graph TD
    subgraph DIPあり - 逆転した設計
        APP2[APP層<br>add.c] -->|抽象に依存| IF2[抽象<br>インターフェース<br>関数宣言ヘッダ]
        DRV2[DRV層<br>drv1.c] -->|抽象を実装| IF2
        FAKE2[フェイク<br>テスト用実装] -->|抽象を実装| IF2
    end
    
    style APP2 fill:#e8f5e9
    style IF2 fill:#e3f2fd,stroke:#1565c0,stroke-width:3px
    style DRV2 fill:#fff3e0
    style FAKE2 fill:#f3e5f5
```

**依存の方向が逆転**しています。APP層は「抽象（ヘッダ＝インターフェース）」に依存し、DRV層もフェイクも同じ抽象を実装します。これにより、テスト時はフェイクに、本番時は実ドライバに切り替え可能になります。

### 本プロジェクトでのDIP

本プロジェクトのコードを見てみましょう。

```mermaid
graph TD
    subgraph sub.h が抽象として機能
        ADDH[add.h<br>int add int a, int b]
        SUBH[sub.h<br>int sub int a, int b<br>void doubleForFake int *a]
    end
    
    subgraph 実装
        ADDC[add.c<br>add の実装<br>doubleForFake を呼ぶ]
        DRV1C[drv1.c<br>sub の実装<br>doubleForFake の実装]
    end
    
    subgraph テスト用フェイク
        FAKE_DFF[FAKE_VOID_FUNC<br>doubleForFake, int*]
        FAKE_ADD[FAKE_VALUE_FUNC<br>int, add, int, int]
    end
    
    ADDC -->|依存| SUBH
    DRV1C -->|実装| SUBH
    FAKE_DFF -->|フェイク実装| SUBH
    FAKE_ADD -->|フェイク実装| ADDH
    
    style SUBH fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style ADDH fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style ADDC fill:#e8f5e9
    style DRV1C fill:#fff3e0
    style FAKE_DFF fill:#f3e5f5
    style FAKE_ADD fill:#f3e5f5
```

- `sub.h` にはインターフェース（関数宣言）のみが記述されています
- `drv1.c` は本番用の実装を提供します
- `FAKE_VOID_FUNC` はテスト用のフェイク実装を提供します
- `add.c` は `sub.h` のインターフェースに依存し、具体的な実装には依存しません

## 5.4 C言語でのDIP実現パターン

C言語にはインターフェースの概念がありませんが、以下の方法でDIPを実現できます。

### パターン1: ヘッダファイルによる宣言と分離

最もシンプルなパターン。ヘッダに宣言だけを書き、実装は別ファイルに分ける。リンク時にどの実装を使うかを切り替えます。

```mermaid
graph TD
    H[driver_if.h<br>void write_register uint32_t addr, uint32_t val] 
    REAL[driver_real.c<br>本番用実装<br>実レジスタに書き込み]
    FAKE[driver_fake.c<br>テスト用実装<br>メモリに書き込み]
    
    H -->|実装| REAL
    H -->|実装| FAKE
    APP[app.c] -->|インクルード| H
    
    style H fill:#e3f2fd,stroke:#1565c0,stroke-width:2px
    style REAL fill:#fff3e0
    style FAKE fill:#f3e5f5
    style APP fill:#e8f5e9
```

本プロジェクトのFFFによるフェイクも、実質的にこのパターンを使っています。

### パターン2: 関数ポインタによる実行時切り替え

```c
// インターフェース定義
typedef struct {
    void (*write)(uint32_t addr, uint32_t val);
    uint32_t (*read)(uint32_t addr);
} DriverInterface;

// アプリケーション層 — 抽象に依存
void app_process(const DriverInterface *drv) {
    drv->write(0x1000, 0xFF);
    uint32_t val = drv->read(0x1000);
}
```

このパターンでは、実行時にインターフェースの実装を差し替えることができます。テスト時にはフェイクの実装を渡し、本番時には実ドライバの実装を渡します。

### パターン3: コンパイルスイッチによる切り替え

```c
#ifdef UNIT_TEST
    #include "driver_fake.h"
#else
    #include "driver_real.h"
#endif
```

最も原始的なパターンですが、既存コードへの導入が容易です。

### どのパターンを選ぶべきか

| パターン | 利点 | 欠点 | 適した場面 |
|---------|------|------|----------|
| パターン1: ヘッダ分離 | シンプル、既存コードに適用しやすい | 実行時に切り替えできない | 組み込みCの多くの場面。FFFとの相性が良い |
| パターン2: 関数ポインタ | 実行時に切り替え可能 | コードが複雑になる | プラグイン的な拡張が必要な場合 |
| パターン3: コンパイルスイッチ | 最も導入が容易 | 条件分岐が増えると保守困難 | 既存コードへの一時的な対応 |

本教材のプロジェクトでは**パターン1（ヘッダ分離）**を採用しています。その理由は以下の通りです。

1. **FFFとの相性が良い**: FFFは同じ関数名のフェイクをマクロで生成するため、ヘッダ分離パターンと自然に組み合わせられる
2. **組み込みCの慣習に合致**: 多くの組み込みプロジェクトでは既にヘッダとソースを分離しており、追加の設計変更が不要
3. **学習コストが低い**: 関数ポインタのパターンは強力だが、C言語初心者には理解が難しい

## 5.5 DIPとテストの関係

DIPの適用度合いによって、テストの容易さが変わります。

```mermaid
graph LR
    subgraph テストの容易さ
        L1[低い] --> L2[中程度] --> L3[高い]
    end
    subgraph DIPの適用度合い
        D1[直接依存<br>ハードウェアに直接アクセス] --> D2[ヘッダ分離<br>ヘッダでインターフェース定義]
        D2 --> D3[関数ポインタ<br>実行時にインターフェース切り替え]
    end
    
    D1 -.-> L1
    D2 -.-> L2
    D3 -.-> L3
    
    style L1 fill:#ffebee
    style L2 fill:#fff8e1
    style L3 fill:#e8f5e9
    style D1 fill:#ffebee
    style D2 fill:#fff8e1
    style D3 fill:#e8f5e9
```

本教材のプロジェクトでは、「ヘッダ分離 + FFFによるリンク時切り替え」を採用しています。これは組み込みCにおいて最も実用的なバランスです。
