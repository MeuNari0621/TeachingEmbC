@echo off
REM Git Conflict Exercise Setup Script
REM 初期ファイルを設定するバッチスクリプト

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "DATA_DIR=%SCRIPT_DIR%data"

echo.
echo ========================================
echo Git Conflict Exercise - Setup
echo ========================================
echo.

REM Create data directory
if not exist "%DATA_DIR%" (
    mkdir "%DATA_DIR%"
    echo [OK] Created data directory
) else (
    echo [OK] Data directory already exists
)

REM Initialize main log file
if not exist "%DATA_DIR%\shared_log.txt" (
    (
        echo ========================================
        echo Git Conflict Exercise Log
        echo ========================================
        echo.
        echo This file will be edited by multiple users.
        echo Conflicts will occur during merges!
        echo.
    ) > "%DATA_DIR%\shared_log.txt"
    echo [OK] Created shared_log.txt
) else (
    echo [OK] shared_log.txt already exists
)

REM Initialize other files
for %%F in (progress.txt notes.txt tasks.txt records.txt) do (
    if not exist "%DATA_DIR%\%%F" (
        (
            echo ========================================
            echo %%F
            echo ========================================
            echo.
            echo This is a shared file for git exercises.
            echo.
        ) > "%DATA_DIR%\%%F"
        echo [OK] Created %%F
    ) else (
        echo [OK] %%F already exists
    )
)

echo.
echo ========================================
echo Setup Complete!
echo ========================================
echo.
echo Next steps:
echo   1. Run: .\generate_conflicts.ps1 -UserName "YourName"
echo   2. Make git commits
echo   3. Create branches and run again
echo   4. Merge branches to practice conflict resolution
echo.
echo For more details, see README.md
echo.
pause
