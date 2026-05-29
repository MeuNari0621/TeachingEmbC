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
| `dbc/` | 契約による設計 + TDD + 実装のサンプル | `docs/md/14_design_by_contract.md`, `docs/md/15_tdd.md`, `docs/md/16_implementation.md` |

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
- 契約による設計 + TDD + 実装の具体例:
  - `dbc/dbc_assert.h` … 契約マクロ定義
  - `dbc/dbc_assert.c` … 違反ハンドラ（差し替え可能）
  - `dbc/battery_monitor.h` … 契約付きヘッダ
  - `dbc/battery_monitor.c` … 契約付き実装
  - `Test/test_dbc_tdd.cpp` … TDD サイクルに沿ったテスト

## オブジェクト指向風サンプルの狙い

`object_oriented/oo_temp_monitor.*` は、以下を C の構造体と関数ポインタで表現します。

- 抽象ポートへの依存
- 状態と操作のカプセル化
- 複数インスタンスの共存
- テスト時の差し替え容易性

このサンプルでは `oo_temp_monitor_t` が ADC ポート、GPIO ポート、閾値、直近の状態をまとめて保持し、`oo_temp_monitor_run()` が 1 サイクル分の処理を実行します。

## 契約による設計（DbC）サンプルの狙い

`dbc/battery_monitor.*` は、以下を契約マクロと TDD で表現します。

- 事前条件（DBC_REQUIRE）: 呼び出し側が満たすべき条件
- 事後条件（DBC_ENSURE）: 関数が保証すべき結果の条件
- 不変条件（DBC_INVARIANT）: 常に成り立つべき条件
- 防御的プログラミング: 契約違反時の安全な振る舞い

`dbc_assert.h` の契約マクロは `NDEBUG` 定義時にゼロコストで除去されます。テスト (`test_dbc_tdd.cpp`) は TDD の Red-Green-Refactor サイクルに沿い、正常系・境界値・異常系・契約違反検出を網羅します。

## GoogleTest / CMake / GCC での確認

このリポジトリでは CMake と GCC で GoogleTest 環境を構成しています。`Test/test_dbc_tdd.cpp` を含む全テストは次のコマンドで確認できます。

```bash
cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure
```

`test_object_oriented` は、通常温度・高温・センサ異常・複数インスタンス・未設定ポートを具体的に検証します。

`test_dbc_tdd` は、ADC電圧変換・状態判定・境界値・契約違反検出・NULLポインタ防御を検証します。
