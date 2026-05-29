/**
 * @file dbc_assert.c
 * @brief 契約違反ハンドラのデフォルト実装
 *
 * テスト環境ではこのハンドラを差し替えて違反を検知する。
 * 実機ではフェイルセーフ動作（リセット等）に置き換える。
 */
#include "dbc_assert.h"

#ifndef NDEBUG

/** 最後の違反情報を記録する（テスト用） */
static const char *g_last_violation_type = 0;
static const char *g_last_violation_expr = 0;
static const char *g_last_violation_file = 0;
static int g_last_violation_line = 0;
static int g_violation_count = 0;

void dbc_violation_handler(const char *type, const char *expr,
                           const char *file, int line)
{
    g_last_violation_type = type;
    g_last_violation_expr = expr;
    g_last_violation_file = file;
    g_last_violation_line = line;
    g_violation_count++;
}

/** テスト用: 違反回数を取得 */
int dbc_get_violation_count(void)
{
    return g_violation_count;
}

/** テスト用: 違反回数をリセット */
void dbc_reset_violations(void)
{
    g_last_violation_type = 0;
    g_last_violation_expr = 0;
    g_last_violation_file = 0;
    g_last_violation_line = 0;
    g_violation_count = 0;
}

/** テスト用: 最後の違反種別を取得 */
const char *dbc_get_last_violation_type(void)
{
    return g_last_violation_type;
}

#endif /* NDEBUG */
