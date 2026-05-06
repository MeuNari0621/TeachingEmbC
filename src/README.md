# src ディレクトリの読み方

`src/` 配下は docs の説明に対応する「実際にビルドできる教材コード」を置く場所です。docs の記述を更新したときは、対応するソースとこの README も合わせて更新します。

## ディレクトリ対応表

| ディレクトリ | 役割 | 主な対応ドキュメント |
|---|---|---|
| `before/` | テストしにくい悪い例 | `docs/md/02_testability.md` |
| `app/` | 純粋関数とアプリ層ロジック | `docs/md/02_testability.md`, `docs/md/08_state_transition.md` |
| `hal/` | ハードウェア抽象化レイヤ | `docs/md/03_port_adapter.md` |
| `autosar/` | AUTOSAR 風の下回り実装例 | `docs/md/12_autosar.md` |
| `object_oriented/` | Cでのオブジェクト指向風サンプル | `docs/md/13_object_oriented.md` |

## 実際のサンプル配置

- テスト容易性の基本:
  - `app/temperature.c` … 純粋関数
  - `app/temp_monitor.c` … HAL 境界を持つオーケストレータ
  - `Test/test_app.cpp` … GoogleTest による純粋関数テスト
  - `Test/test_drv.cpp` … FFF を使った副作用コードのテスト
- オブジェクト指向風設計の具体例:
  - `object_oriented/oo_temp_monitor.h`
  - `object_oriented/oo_temp_monitor.c`
  - `Test/test_object_oriented.cpp`

## オブジェクト指向風サンプルの狙い

`object_oriented/oo_temp_monitor.*` は、以下を C の構造体と関数ポインタで表現します。

- 抽象ポートへの依存
- 状態と操作のカプセル化
- 複数インスタンスの共存
- テスト時の差し替え容易性

このサンプルでは `oo_temp_monitor_t` が ADC ポート、GPIO ポート、閾値、直近の状態をまとめて保持し、`oo_temp_monitor_run()` が 1 サイクル分の処理を実行します。

## GoogleTest / CMake / GCC での確認

このリポジトリでは CMake と GCC で GoogleTest 環境を構成しています。`Test/test_object_oriented.cpp` を含む全テストは次のコマンドで確認できます。

```bash
cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
```

`test_object_oriented` は、通常温度・高温・センサ異常・複数インスタンス・未設定ポートを具体的に検証します。
