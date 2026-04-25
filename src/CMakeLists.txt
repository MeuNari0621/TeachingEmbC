# src/CMakeLists.txt

# === AUTOSAR 風 MCAL 層（教材向け簡略版）===
set(AUTOSAR_MCAL_SOURCES
    autosar/Adc.c
    autosar/Dio.c
    autosar/Port.c
)

add_library(AutosarMcal STATIC ${AUTOSAR_MCAL_SOURCES})
target_include_directories(AutosarMcal PUBLIC
    autosar
)

# === アプリケーション層（純粋関数＋ビジネスロジック）===
set(APP_SOURCES
    app/temperature.c
    app/temp_monitor.c
    app/temp_alarm_fsm.c
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
target_include_directories(HalLibrary
    PUBLIC
        hal
    PRIVATE
        autosar
)
target_link_libraries(HalLibrary PRIVATE AutosarMcal)

# アプリケーション → HAL の依存
target_link_libraries(AppLibrary PUBLIC HalLibrary)
