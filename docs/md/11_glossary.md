# 付録: 用語集

| 用語 | 英語 | 説明 |
|------|------|------|
| TDD | Test-Driven Development | テスト駆動開発。テストを先に書き、実装を後から書く開発手法 |
| Red-Green-Refactor | — | TDDの3ステップ。テスト失敗(赤)→テスト成功(緑)→リファクタリング |
| Google Test | — | Google製のC++テストフレームワーク。`TEST`, `TEST_F`, `EXPECT_EQ` 等のマクロを提供 |
| FFF | Fake Function Framework | C/C++用のフェイク関数生成フレームワーク。ヘッダ1つで導入可能 |
| フェイク | Fake | テスト用の簡易実装。FFFが生成するスタブ+スパイの機能を持つ |
| スタブ | Stub | 固定値を返すテストダブル。FFFでは `return_val` で設定 |
| モック | Mock | 期待される呼び出しを事前に設定し、違反を検出するテストダブル |
| スパイ | Spy | 呼び出しを記録するテストダブル。FFFでは `call_count`, `arg0_val` 等 |
| テストダブル | Test Double | テスト用の代替実装の総称。フェイク、スタブ、モック、スパイの上位概念 |
| フィクスチャ | Test Fixture | テストの前提条件（初期化・後始末）を共有する仕組み。`TEST_F` で使用 |
| アサーション | Assertion | テストの検証ポイント。`EXPECT_EQ`, `ASSERT_TRUE` 等 |
| 純粋関数 | Pure Function | 同じ入力に対して常に同じ出力を返し、外部状態を変更しない関数 |
| 副作用 | Side Effect | 関数が外部の状態を変更すること。ハードウェアアクセス、グローバル変数の変更等 |
| HAL | Hardware Abstraction Layer | ハードウェア抽象化レイヤ。ハードウェアアクセスを関数でラップし、実装を差し替え可能にする |
| ポートアダプタ | Port-Adapter | ヘキサゴナルアーキテクチャとも呼ばれる。コアロジックと外部依存を分離する設計パターン |
| ヘキサゴナルアーキテクチャ | Hexagonal Architecture | ポートアダプタパターンの別名。アプリケーションコアを中心に、ポートとアダプタで外部と接続 |
| SOLID | — | オブジェクト指向設計の5原則。SRP, OCP, LSP, ISP, DIP |
| SRP | Single Responsibility Principle | 単一責任の原則。関数やモジュールは1つの責任のみ持つ |
| OCP | Open-Closed Principle | 開放閉鎖の原則。拡張に開いて修正に閉じる |
| LSP | Liskov Substitution Principle | リスコフの置換原則。基底型を派生型で置換しても正しく動作する |
| ISP | Interface Segregation Principle | インターフェース分離の原則。不要なインターフェースへの依存を避ける |
| DIP | Dependency Inversion Principle | 依存性逆転の原則。上位→抽象←下位 の依存関係にする |
| extern "C" | — | C++からC関数を呼び出すための宣言。名前マングリングを抑制する |
| 名前マングリング | Name Mangling | C++コンパイラが関数名を装飾する仕組み。オーバーロード等を実現するため |
| CMake | — | クロスプラットフォームのビルドシステム。`CMakeLists.txt` で設定 |
| FetchContent | — | CMakeの機能。外部ライブラリ（Google Test等）を自動でダウンロード・ビルド |
| ADC | Analog-to-Digital Converter | アナログ信号をデジタル値に変換するペリフェラル |
| GPIO | General Purpose Input/Output | 汎用入出力ピン。LED制御やスイッチ読み取りに使用 |
| ペリフェラル | Peripheral | マイコンの周辺機器。ADC, GPIO, UART, SPI, I2C 等 |
| LLM | Large Language Model | 大規模言語モデル。ChatGPT, Claude, GitHub Copilot 等のベースとなるAI |
| ハルシネーション | Hallucination | AIがもっともらしいが誤った情報を生成すること |
| FIRST原則 | — | 良いテストの特性。Fast, Independent, Repeatable, Self-validating, Timely |
| 境界値テスト | Boundary Value Test | 入力の境界（最小値、最大値、閾値前後）をテストする手法 |
| リファクタリング | Refactoring | 外部動作を変えずにコードの内部構造を改善すること |
| オーケストレータ | Orchestrator | 複数の関数を組み合わせて処理フローを制御する関数 |
| ベアメタル | Bare Metal | OSなしでマイコン上で直接動作するソフトウェア |
| RTOS | Real-Time Operating System | リアルタイムOS。組み込み向けの軽量OS |
| stdint.h | — | 固定幅整数型（uint8_t, int16_t等）を定義するC標準ヘッダ |
| volatile | — | コンパイラの最適化を抑制するC言語の修飾子。ハードウェアレジスタアクセスに使用 |
| クロスコンパイラ | Cross Compiler | ホストと異なるアーキテクチャ向けのコンパイラ（例: arm-none-eabi-gcc） |
| エンディアン | Endianness | バイトの並び順。リトルエンディアン（x86）とビッグエンディアン |
| FPU | Floating Point Unit | 浮動小数点演算ユニット。FPUがないMCUでは浮動小数点を避ける |
| GMock | Google Mock | Google製のモックフレームワーク。C++クラスのモックを自動生成 |
