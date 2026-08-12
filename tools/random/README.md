# Git Conflict Exercise Generator

複数人でgitを使う際の**コンフリクト発生と解決の練習**に使うツールです。

## 概要

このスクリプトは、実行するたびに**ランダムに**ファイルを編集または新規作成します。
複数人が異なるブランチで実行した後、マージするとコンフリクトが発生します。

## 機能

✓ **ランダムなファイル編集/作成** - 毎回異なる操作を実行
✓ **複数人対応** - ユーザー名を指定して実行
✓ **コンフリクト発生** - 複数のブランチでマージするとコンフリクトが必ず発生
✓ **実践的な練習** - 実際のgitワークフローをシミュレート

## 使い方

### PowerShell版（Windows）

```powershell
cd tools/random

# 基本的な使い方（デフォルトユーザー、1回実行）
.\generate_conflicts.ps1

# ユーザー名を指定
.\generate_conflicts.ps1 -UserName "Alice"

# 複数回実行
.\generate_conflicts.ps1 -UserName "Bob" -Count 5

# 両方指定
.\generate_conflicts.ps1 -UserName "Charlie" -Count 3
```

### Bash版（Linux/Mac）

```bash
cd tools/random

# 基本的な使い方（デフォルトユーザー、1回実行）
chmod +x generate_conflicts.sh
./generate_conflicts.sh

# ユーザー名を指定
./generate_conflicts.sh --user Alice

# 複数回実行
./generate_conflicts.sh --user Bob --count 5

# 短いオプション
./generate_conflicts.sh -u Charlie -c 3
```

## 演習シナリオ

### シナリオ1: 基本的なコンフリクト

```bash
# 初期化
git init my-repo
cd my-repo

# メインブランチで変更1
.\generate_conflicts.ps1 -UserName "Person1"
git add .
git commit -m "Person1's changes"

# ブランチを作成して変更2
git checkout -b feature-branch
.\generate_conflicts.ps1 -UserName "Person2"
git add .
git commit -m "Person2's changes"

# メインブランチに戻って別の変更
git checkout main
.\generate_conflicts.ps1 -UserName "Person3"
git add .
git commit -m "Person3's changes"

# マージしてコンフリクト発生
git merge feature-branch
# → コンフリクト解決の練習！
```

### シナリオ2: 複数ブランチでのコンフリクト

```bash
#複数のブランチを作成して、それぞれで変更
git checkout -b branch1
.\generate_conflicts.ps1 -UserName "User1" -Count 2
git add data/
git commit -m "Branch1 changes"

git checkout -b branch2
.\generate_conflicts.ps1 -UserName "User2" -Count 2
git add data/
git commit -m "Branch2 changes"

# メインブランチで別の変更
git checkout main
.\generate_conflicts.ps1 -UserName "User3" -Count 2
git add data/
git commit -m "Main changes"

# マージしてコンフリクト解決
git merge branch1
# → コンフリクト解決
git merge branch2
# → さらにコンフリクト解決
```

## 生成されるファイル

### `data/shared_log.txt`
複数人が追記する共通ログファイル。最もコンフリクトが起こりやすい。

### `data/progress.txt`
進捗情報を記録するファイル。

### `data/notes.txt`, `data/tasks.txt`, `data/records.txt`
その他の共有ファイル。

### `data/file_*.txt`
スクリプト実行時にランダムに作成される新規ファイル。

## コンフリクトが発生する仕組み

1. **共有ファイルへの同時編集**
   - 異なるブランチで同じファイル（例：`shared_log.txt`）に行を追加
   - マージ時にどちらの行を優先するかで競合

2. **ランダムなファイル生成**
   - ファイル名がランダムなので、時々同じ名前のファイルが生成される可能性
   - または異なるファイルを作成することで、ブランチ間の差異を増加

3. **実際のチーム開発をシミュレート**
   - 複数人が同時に異なる部分を編集
   - 同じ部分を編集してコンフリクト発生

## トラブルシューティング

### PowerShellスクリプトが実行できない
```powershell
# 実行ポリシーを一時的に変更
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
```

### Bashスクリプトが実行できない
```bash
# 実行権限を追加
chmod +x generate_conflicts.sh
```

## 学習ポイント

✓ コンフリクト発生時のメッセージ理解
✓ コンフリクトマーカー（`<<<<<<<`, `=======`, `>>>>>>>`）の意味
✓ テキストエディタでの手動解決
✓ `git status` でのコンフリクト状態確認
✓ `git add`, `git commit` でのコンフリクト解決
✓ `git merge --abort` での中止
✓ `git rebase` による別のマージ方法

## その他のコマンド

```bash
# コンフリクト状態を確認
git status

# マージをキャンセル
git merge --abort

# 特定のファイルを選択
git checkout --ours data/shared_log.txt   # 現在のブランチを選択
git checkout --theirs data/shared_log.txt # マージ元を選択

# マージの履歴を確認
git log --oneline --graph --all
```

---

**Happy Git Practicing! 🚀**
