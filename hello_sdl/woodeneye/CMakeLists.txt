set(CPP_FILES
        src/main.cpp
)

set(CPP_MODULES
        src/application.ixx
        src/types.ixx
)

add_executable(woodeneye
        ${CPP_FILES}
        ${CPP_MODULES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

target_link_libraries(woodeneye PRIVATE
        SDL3::SDL3 EnTT::EnTT
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

target_compile_features(woodeneye PRIVATE cxx_std_26)

target_sources(woodeneye
        PRIVATE ${CPP_FILES}
        PRIVATE FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

# Copy resources (only if the res/ folder exists)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    add_custom_command(TARGET woodeneye POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:woodeneye>/res
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:woodeneye>/res
            COMMENT "Copying resources"
    )
endif ()

# 设置输出目录
set_target_properties(woodeneye PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/woodeneye"
)