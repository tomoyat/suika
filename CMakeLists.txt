cmake_minimum_required(VERSION 3.16)
project(suika)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib)

add_executable(suika main.cpp)
target_link_libraries(suika lib)

# GoogleTestをダウンロードしてビルド
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# テスト用のサブディレクトリを追加
enable_testing()
add_subdirectory(test)
