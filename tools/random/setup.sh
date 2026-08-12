#!/bin/bash
# Git Conflict Exercise Setup Script
# 初期ファイルを設定するシェルスクリプト

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/data"

echo ""
echo "========================================"
echo "Git Conflict Exercise - Setup"
echo "========================================"
echo ""

# Create data directory
if [ ! -d "$DATA_DIR" ]; then
    mkdir -p "$DATA_DIR"
    echo "[OK] Created data directory"
else
    echo "[OK] Data directory already exists"
fi

# Initialize main log file
if [ ! -f "$DATA_DIR/shared_log.txt" ]; then
    cat > "$DATA_DIR/shared_log.txt" << 'EOF'
========================================
Git Conflict Exercise Log
========================================

This file will be edited by multiple users.
Conflicts will occur during merges!

EOF
    echo "[OK] Created shared_log.txt"
else
    echo "[OK] shared_log.txt already exists"
fi

# Initialize other files
for FILE in progress.txt notes.txt tasks.txt records.txt; do
    if [ ! -f "$DATA_DIR/$FILE" ]; then
        cat > "$DATA_DIR/$FILE" << EOF
========================================
$FILE
========================================

This is a shared file for git exercises.

EOF
        echo "[OK] Created $FILE"
    else
        echo "[OK] $FILE already exists"
    fi
done

echo ""
echo "========================================"
echo "Setup Complete!"
echo "========================================"
echo ""
echo "Next steps:"
echo "  1. Run: ./generate_conflicts.sh --user YourName"
echo "  2. Make git commits"
echo "  3. Create branches and run again"
echo "  4. Merge branches to practice conflict resolution"
echo ""
echo "For more details, see README.md"
echo ""
