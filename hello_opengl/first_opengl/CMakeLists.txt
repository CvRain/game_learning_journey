set(SUBPROJECT_NAME "first_opengl")

set(CPP_FILES
        src/main.cpp
)

set(CPP_MODULES
        src/application.ixx
)

add_executable(${SUBPROJECT_NAME}
        ${CPP_FILES}
        ${CPP_MODULES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(${SUBPROJECT_NAME} PRIVATE
        SDL3::SDL3 glad::glad)

target_compile_features(${SUBPROJECT_NAME} PRIVATE cxx_std_26)

target_sources(${SUBPROJECT_NAME}
        PRIVATE ${CPP_FILES}
        PRIVATE FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

# Copy resources (only if the res/ folder exists)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    add_custom_command(TARGET ${SUBPROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
            COMMENT "Copying resources"
    )
endif ()

# 设置输出目录
set_target_properties(${SUBPROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SUBPROJECT_NAME}"
)