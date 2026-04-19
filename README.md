# TeachingEmbC

組み込みC向けのTDD教材です。Google Test、FFF、HAL分離、AI駆動開発の考え方を、静的ドキュメントとサンプルコードで学べます。

## ドキュメント

- メインドキュメント: docs/index.html
- 状態遷移の具体例: docs/md/10_state_transition_example.md
- ISRの具体例: docs/md/11_isr_example.md
- HTMLセキュリティチェックリスト: docs/md/09_html_security_checklist.md

## ドキュメント運用メモ

- docs/index.html は単体で閲覧できる静的HTMLです
- Mermaid は docs/assets/vendor/mermaid.min.js にローカル固定版を配置しています
- ページ内の振る舞いは docs/assets/index.js に分離しています
- 用語集は左側ナビの「用語集」タブからポップアップ表示します

## HTML確認手順

1. docs/index.html をテキストエディタで確認する
2. docs/md/09_html_security_checklist.md に沿って点検する
3. 必要ならネット遮断状態で docs/index.html をブラウザ確認する