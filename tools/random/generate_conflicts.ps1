# Git Conflict Exercise - Random File Generator
# 複数人が実行するとコンフリクトが発生するランダムなファイル生成スクリプト
# Usage: .\generate_conflicts.ps1 -UserName "John" -Count 3

param(
    [Parameter(Mandatory=$false)]
    [string]$UserName = "user_$(Get-Random -Minimum 1000 -Maximum 9999)",
    
    [Parameter(Mandatory=$false)]
    [int]$Count = 1
)

$baseDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$dataDir = Join-Path $baseDir "data"
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss.fff"

# ファイル名のリスト（複数人でアクセスしやすいファイル）
$commonFiles = @{
    "shared_log.txt" = "LOG"
    "progress.txt" = "PROGRESS"
    "notes.txt" = "NOTES"
    "tasks.txt" = "TASKS"
    "records.txt" = "RECORDS"
}

# ランダムなコンテンツ生成用の単語リスト
$actions = @("added", "fixed", "updated", "implemented", "refactored", "tested", "reviewed", "merged")
$features = @("feature", "bug fix", "enhancement", "documentation", "optimization", "cleanup", "integration", "migration")
$components = @("API", "Database", "UI", "Core", "Service", "Module", "Component", "Library", "Function", "Method")

# Initialize data directory if not exists
if (!(Test-Path $dataDir)) {
    New-Item -ItemType Directory -Path $dataDir | Out-Null
}

# メイン処理：指定回数ループしてランダムにファイル操作
for ($i = 1; $i -le $Count; $i++) {
    # ランダムに共有ファイルを選択
    $selectedFile = ($commonFiles.Keys | Get-Random)
    $filePath = Join-Path $dataDir $selectedFile
    
    # ランダムな処理を選択（編集 or 新規ファイル作成）
    $operation = @("edit", "createNew") | Get-Random
    
    if ($operation -eq "createNew") {
        # 新規ファイル作成
        $newFileName = "file_$(Get-Random -Minimum 10000 -Maximum 99999).txt"
        $newFilePath = Join-Path $dataDir $newFileName
        
        $content = @"
User: $UserName
Created: $timestamp
Task: $($features | Get-Random)

Details:
- Component: $($components | Get-Random)
- Status: In Progress
- Changes: $($actions | Get-Random) $($features | Get-Random)

---
"@
        
        Add-Content -Path $newFilePath -Value $content -Encoding UTF8
        Write-Host "[CREATED] $newFileName" -ForegroundColor Green
    }
    else {
        # 既存ファイルに追記（コンフリクト発生の原因）
        if (!(Test-Path $filePath)) {
            # ファイルが存在しない場合は初期化
            @"
========================================
Git Conflict Exercise Log
========================================

"@ | Set-Content -Path $filePath -Encoding UTF8
        }
        
        $lineContent = @"
[$timestamp] $UserName - $($actions | Get-Random) $($features | Get-Random) ($($components | Get-Random))
"@
        
        Add-Content -Path $filePath -Value $lineContent -Encoding UTF8
        Write-Host "[EDITED] $selectedFile" -ForegroundColor Cyan
    }
}

Write-Host ""
Write-Host "✓ Generated $Count random changes for '$UserName'" -ForegroundColor Green
Write-Host "  Files are in: $dataDir" -ForegroundColor Gray
Write-Host ""
Write-Host "Tips for Git Conflict Exercise:" -ForegroundColor Yellow
Write-Host "  1. Run this script multiple times with different users"
Write-Host "  2. Switch between branches and create different changes"
Write-Host "  3. Merge branches to trigger conflicts"
Write-Host "  4. Practice resolving conflicts!"
