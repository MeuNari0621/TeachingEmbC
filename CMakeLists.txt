cmake_minimum_required(VERSION 3.29)

project(MyMixedProject LANGUAGES C CXX)

include(CTest)

# C言語ソースのサブディレクトリを追加
add_subdirectory(src)

# C++ソースのサブディレクトリを追加
add_subdirectory(Test)

# C++17の標準を使用する（必要に応じて変更可能）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# C言語の標準を指定する（オプション）
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
