cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# ソリューションエクスプローラーにCMake用のフォルダを別に用意する
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# edupt本体プロジェクト
project(edupt VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
add_executable(edupt main.cpp)

# インストール
install (TARGETS edupt DESTINATION ${CMAKE_SOURCE_DIR}/bin)