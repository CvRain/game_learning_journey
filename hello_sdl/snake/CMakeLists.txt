set(CPP_FILES
        src/main.cpp
)

set(CPP_MODULES
        src/application.ixx
)

add_executable(snake
        ${CPP_FILES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)

target_link_libraries(snake PRIVATE
        SDL3::SDL3
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

target_compile_features(snake PRIVATE cxx_std_26)

target_sources(snake
        PRIVATE ${CPP_FILES}
        PRIVATE FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

add_custom_command(TARGET snake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:snake>/res
        COMMENT "Copying resources"
)

# 设置输出目录
set_target_properties(snake PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/07_streaming_texture"
)