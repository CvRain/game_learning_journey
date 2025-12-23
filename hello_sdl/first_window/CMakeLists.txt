# first_window 子项目

# 收集源文件
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 创建可执行文件
# executable
add_executable(first_window ${SOURCES})

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(first_window PRIVATE SDL3::SDL3)
# 设置输出目录
set_target_properties(first_window PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
