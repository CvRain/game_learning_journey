# 收集源文件
set(CPP_FILES app.cpp
)

set(CPP_MODULES
        interface.ixx
)
# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

# 创建可执行文件
# executable
add_executable(entt_hello_world ${CPP_FILES})

add_library(engine_module)
target_sources(engine_module
        PRIVATE ${CPP_FILES}
        PUBLIC FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_link_libraries(engine_module PRIVATE SDL3::SDL3 EnTT::EnTT)

target_link_directories(entt_hello_world PRIVATE engine_module)

# 设置输出目录
set_target_properties(entt_hello_world PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)