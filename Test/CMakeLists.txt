# Test/CMakeLists.txt

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

if (WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

# === test_temperature（純粋関数テスト — フェイク不要） ===
add_executable(test_temperature test_app.cpp)
target_link_libraries(test_temperature gtest_main AppLibrary)
include(GoogleTest)
gtest_discover_tests(test_temperature)

# === test_temp_monitor（副作用テスト — FFF でHALをフェイク化） ===
# HAL のリンクは不要（FFF が関数定義を生成するため）
add_executable(test_temp_monitor test_drv.cpp)
target_link_libraries(test_temp_monitor gtest_main)
# AppLibrary から温度変換関数を取得し、HAL は FFF で差し替え
target_include_directories(test_temp_monitor PRIVATE
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_SOURCE_DIR}/src/hal
    ${CMAKE_CURRENT_SOURCE_DIR}
)
# temp_monitor.c をテストバイナリに直接含める（HAL はリンクしない）
target_sources(test_temp_monitor PRIVATE
    ${CMAKE_SOURCE_DIR}/src/app/temp_monitor.c
    ${CMAKE_SOURCE_DIR}/src/app/temperature.c
)
gtest_discover_tests(test_temp_monitor)

# === test_event_fsm（状態遷移・割り込み・イベント駆動のテスト） ===
add_executable(test_event_fsm test_event_fsm.cpp)
target_link_libraries(test_event_fsm gtest_main AppLibrary)
gtest_discover_tests(test_event_fsm)

# === test_state_transition（純粋関数化した状態遷移のテスト） ===
add_executable(test_state_transition test_state_transition.cpp)
target_link_libraries(test_state_transition gtest_main AppLibrary)
gtest_discover_tests(test_state_transition)

# === test_autosar_hal（AUTOSAR風MCALとHALアダプタのテスト） ===
add_executable(test_autosar_hal test_autosar_hal.cpp)
target_link_libraries(test_autosar_hal gtest_main HalLibrary AutosarMcal)
target_include_directories(test_autosar_hal PRIVATE
    ${CMAKE_SOURCE_DIR}/src/hal
    ${CMAKE_SOURCE_DIR}/src/autosar
)
gtest_discover_tests(test_autosar_hal)
