#!/bin/bash
# Git Conflict Exercise - Random File Generator
# 複数人が実行するとコンフリクトが発生するランダムなファイル生成スクリプト
# Usage: ./generate_conflicts.sh --user John --count 3

# パラメータ処理
USER_NAME=""
COUNT=1

while [[ $# -gt 0 ]]; do
    case $1 in
        --user|--name|-u|-n)
            USER_NAME="$2"
            shift 2
            ;;
        --count|-c)
            COUNT="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# デフォルトユーザー名
if [ -z "$USER_NAME" ]; then
    USER_NAME="user_$((RANDOM % 9000 + 1000))"
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/data"
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S.%3N')

# ファイル名のリスト（複数人でアクセスしやすいファイル）
COMMON_FILES=(
    "shared_log.txt"
    "progress.txt"
    "notes.txt"
    "tasks.txt"
    "records.txt"
)

# ランダムなコンテンツ生成用の単語リスト
ACTIONS=("added" "fixed" "updated" "implemented" "refactored" "tested" "reviewed" "merged")
FEATURES=("feature" "bug fix" "enhancement" "documentation" "optimization" "cleanup" "integration" "migration")
COMPONENTS=("API" "Database" "UI" "Core" "Service" "Module" "Component" "Library" "Function" "Method")

# Data directory を作成
mkdir -p "$DATA_DIR"

# ランダム要素を取得するヘルパー関数
get_random_element() {
    local array=("$@")
    local index=$((RANDOM % ${#array[@]}))
    echo "${array[$index]}"
}

# メイン処理：指定回数ループしてランダムにファイル操作
for ((i = 1; i <= COUNT; i++)); do
    # ランダムに共有ファイルを選択
    SELECTED_FILE=$(get_random_element "${COMMON_FILES[@]}")
    FILE_PATH="$DATA_DIR/$SELECTED_FILE"
    
    # ランダムな処理を選択（編集 or 新規ファイル作成）
    OPERATION=$([ $((RANDOM % 2)) -eq 0 ] && echo "edit" || echo "createNew")
    
    if [ "$OPERATION" = "createNew" ]; then
        # 新規ファイル作成
        NEW_FILE_NAME="file_$((RANDOM % 90000 + 10000)).txt"
        NEW_FILE_PATH="$DATA_DIR/$NEW_FILE_NAME"
        
        cat > "$NEW_FILE_PATH" << EOF
User: $USER_NAME
Created: $TIMESTAMP
Task: $(get_random_element "${FEATURES[@]}")

Details:
- Component: $(get_random_element "${COMPONENTS[@]}")
- Status: In Progress
- Changes: $(get_random_element "${ACTIONS[@]}") $(get_random_element "${FEATURES[@]}")

---
EOF
        
        echo -e "\033[32m[CREATED] $NEW_FILE_NAME\033[0m"
    else
        # 既存ファイルに追記（コンフリクト発生の原因）
        if [ ! -f "$FILE_PATH" ]; then
            # ファイルが存在しない場合は初期化
            cat > "$FILE_PATH" << 'EOF'
========================================
Git Conflict Exercise Log
========================================

EOF
        fi
        
        echo "[$TIMESTAMP] $USER_NAME - $(get_random_element "${ACTIONS[@]}") $(get_random_element "${FEATURES[@]}") ($(get_random_element "${COMPONENTS[@]}"))" >> "$FILE_PATH"
        
        echo -e "\033[36m[EDITED] $SELECTED_FILE\033[0m"
    fi
done

echo ""
echo -e "\033[32m✓ Generated $COUNT random changes for '$USER_NAME'\033[0m"
echo -e "\033[37m  Files are in: $DATA_DIR\033[0m"
echo ""
echo -e "\033[33mTips for Git Conflict Exercise:\033[0m"
echo "  1. Run this script multiple times with different users"
echo "  2. Switch between branches and create different changes"
echo "  3. Merge branches to trigger conflicts"
echo "  4. Practice resolving conflicts!"
