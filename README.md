# TeachingEmbC

組み込みC向けのTDD教材です。テスト容易性の原則から入り、ポートアダプタ、Google Test、FFF、AI駆動開発、状態遷移、ISRへと進む流れで、静的ドキュメントとサンプルコードを学べます。

## ドキュメント

- メインドキュメント: docs/index.html
- 第8章 状態遷移の設計とテスト: docs/md/08_state_transition.md
- 第9章 ISR設計と副作用の分離: docs/md/09_isr.md
- 第12章 AUTOSAR準拠の下回り設計: docs/md/12_autosar.md
- 第10章 HTMLセキュリティチェックリスト: docs/md/10_html_security_checklist.md

## ドキュメント運用メモ

- docs/index.html は単体で閲覧できる静的HTMLです
- Mermaid は docs/assets/vendor/mermaid.min.js にローカル固定版を配置しています
- ページ内の振る舞いは docs/assets/index.js に分離しています
- 用語集は左側ナビの「用語集」タブからポップアップ表示します

## 検証状況

- CMake ビルド: `cmake --build build`
- CTest 実行: `ctest --test-dir build --output-on-failure`
- 現在のホストテスト構成: `test_temperature`, `test_temp_monitor`, `test_event_fsm`, `test_state_transition`, `test_autosar_hal`
- 最新確認結果: 34 tests passed, 0 tests failed out of 34
- VS Code CMake Tools GUI: `default` preset を選択後、Build と Run Tests の両方を確認済み

## VS Code GUI でのビルドとテスト

1. ワークスペースを開き、推奨拡張機能の `ms-vscode.cmake-tools` と `ms-vscode.cpptools` を有効にする
2. CMake Tools が `CMakePresets.json` の `default` preset を読み込んだら、ステータスバーの Configure を実行する
3. ステータスバーの Build からビルドし、Testing ビューまたは CMake の Run Tests でテストを実行する
4. Testing ビューで 34 テストの PASS/FAIL を確認する

- `CMakePresets.json` は GUI 用の configure/build/test preset を定義します
- `.vscode/settings.json` は presets 利用と configure-on-open を固定します
- `.vscode/extensions.json` は必要な拡張機能をワークスペース推奨として表示します

## Google Tests ビューでのテスト実行

1. 推奨拡張機能の `davidschuldenfrei.gtest-adapter` を有効にする
2. 一度 `default` preset でビルドし、`build/Test/` 配下にテスト実行ファイルを生成する
3. Test Explorer で `test_temperature`, `test_temp_monitor`, `test_event_fsm`, `test_state_transition`, `test_autosar_hal` を読み込んで実行する

- `.vscode/settings.json` の `gtest-adapter.debugConfig` で 5 本の launch 構成名を列挙しています
- `.vscode/launch.json` の `GTest: ...` 構成が、それぞれ `build/Test/*.exe` の実行ファイルを指します
- Windows では MinGW の `C:/bin/mingw64/bin/gdb.exe` を使ってデバッグします

### Test Explorer での確認手順

1. ビルド後に Test Explorer を開き、必要ならテスト一覧の再読込を実行する
2. 5つの実行ファイル単位でテストが見えることを確認する
3. まず `test_temperature` か `test_state_transition` の単体実行で動作確認する
4. 最後に全体実行し、合計 34 テストが通ることを確認する

- テスト一覧が空のままなら、再ビルド後に再読込を実行する
- `build/Test/` に実行ファイルがなければ、先に CMake Tools かコマンドラインでビルドする

### よくある詰まりどころ

- `No configure preset is active for this CMake project` と出たら、CMake Tools で configure preset に `default` を選び直してから Configure を実行する
- Testing ビューにテストが出ない、または古い CMake 状態を拾っている場合は、Configure を再実行する。必要なら `build/` を作り直す
- このリポジトリのルート CMake は `include(CTest)` を使っており、GUI 側のテスト検出もこの前提で動く
- Google Tests ビューが空のままなら、まずビルド済みかどうかと `build/Test/` 配下に実行ファイルがあるかを確認する
- `You first need to define a debug configuration, for your tests` と出る場合は、`.vscode/launch.json` が読み込まれていないので、ワークスペース再読込後に再度 Refresh する
- `gtest-adapter.debugConfig` は拡張側の設定スキーマ不整合によりエディタ上で警告表示されることがありますが、拡張の実装自体は配列を正しく読み取ります

## HTML確認手順

1. docs/index.html をテキストエディタで確認する
2. docs/md/10_html_security_checklist.md に沿って点検する
3. 必要ならネット遮断状態で docs/index.html をブラウザ確認する
