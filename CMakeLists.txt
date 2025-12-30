cmake_minimum_required(VERSION 4.0)

project(GameLearningJourney VERSION 0.1.0 LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# 导出编译命令用于 IDE 支持
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加子目录
add_subdirectory(hello_sdl)
add_subdirectory(hello_opengl)