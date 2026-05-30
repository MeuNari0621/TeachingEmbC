/**
 * @file dbc_assert.h
 * @brief 契約による設計（Design by Contract）用アサーションマクロ
 *
 * 事前条件・事後条件・不変条件を表明するマクロを提供する。
 * NDEBUG 定義時はアサーションを無効化できる（リリースビルド向け）。
 */
#ifndef DBC_ASSERT_H
#define DBC_ASSERT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 契約違反時に呼ばれるハンドラ（弱シンボル、差し替え可能）
 * @param type  違反種別（"PRE", "POST", "INV"）
 * @param expr  違反した条件式の文字列
 * @param file  ファイル名
 * @param line  行番号
 */
void dbc_violation_handler(const char *type, const char *expr,
                           const char *file, int line);

#ifndef NDEBUG

/** 事前条件（Precondition）: 関数呼び出し側が満たすべき条件 */
#define DBC_REQUIRE(expr) \
    do { if (!(expr)) dbc_violation_handler("PRE", #expr, __FILE__, __LINE__); } while(0)

/** 事後条件（Postcondition）: 関数が保証すべき結果の条件 */
#define DBC_ENSURE(expr) \
    do { if (!(expr)) dbc_violation_handler("POST", #expr, __FILE__, __LINE__); } while(0)

/** 不変条件（Invariant）: 常に成り立つべき条件 */
#define DBC_INVARIANT(expr) \
    do { if (!(expr)) dbc_violation_handler("INV", #expr, __FILE__, __LINE__); } while(0)

#else /* NDEBUG */

#define DBC_REQUIRE(expr)   ((void)0)
#define DBC_ENSURE(expr)    ((void)0)
#define DBC_INVARIANT(expr) ((void)0)

#endif /* NDEBUG */

#ifdef __cplusplus
}
#endif

#endif /* DBC_ASSERT_H */
