set(CPP_FILES
        src/app.cpp
)

set(CPP_MODULES
        src/Application.ixx
)

add_executable(primitives
        ${CPP_FILES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(primitives PRIVATE SDL3::SDL3)
target_compile_features(primitives PRIVATE cxx_std_26)

target_sources(primitives
        PRIVATE ${CPP_FILES}
        PUBLIC FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

# 设置输出目录
set_target_properties(primitives PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)