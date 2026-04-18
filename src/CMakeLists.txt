# src/CMakeLists.txt

# === アプリケーション層（純粋関数＋ビジネスロジック）===
set(APP_SOURCES
    app/temperature.c
    app/temp_monitor.c
)

add_library(AppLibrary STATIC ${APP_SOURCES})
target_include_directories(AppLibrary PUBLIC
    app
    hal
)

# === HAL 層（ハードウェア抽象化）===
set(HAL_SOURCES
    hal/hal_adc.c
    hal/hal_gpio.c
)

add_library(HalLibrary STATIC ${HAL_SOURCES})
target_include_directories(HalLibrary PUBLIC
    hal
)

# アプリケーション → HAL の依存
target_link_libraries(AppLibrary PUBLIC HalLibrary)
