set(CPP_FILES
        src/app.cpp
)

set(CPP_MODULES
        src/Application.ixx
)

add_executable(some_rectangle
        ${CPP_FILES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(some_rectangle PRIVATE SDL3::SDL3)
target_compile_features(some_rectangle PRIVATE cxx_std_26)

target_sources(some_rectangle
        PRIVATE ${CPP_FILES}
        PRIVATE FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

# 设置输出目录
set_target_properties(some_rectangle PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)