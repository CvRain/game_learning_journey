set(SUBPROJECT_NAME "opengl_sandbox")

set(CPP_FILES
        src/main.cpp
)

set(CPP_MODULES
        src/window.ixx
        src/app.ixx
        src/sandbox.ixx
        src/file_operation.ixx
        src/shader.ixx
)

set(SHADER_FILES
        res/shader/first_vert.glsl
        res/shader/first_frag.glsl
)

add_executable(${SUBPROJECT_NAME}
        ${CPP_FILES}
        ${CPP_MODULES}
        ${SHADER_FILES}
)

# Find SDL3 and link
find_package(SDL3 CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${SUBPROJECT_NAME} PRIVATE
        SDL3::SDL3 glad::glad glm::glm)

target_include_directories(${SUBPROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

target_compile_features(${SUBPROJECT_NAME} PRIVATE cxx_std_26)

target_sources(${SUBPROJECT_NAME}
        PRIVATE ${CPP_FILES}
        PRIVATE FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

# Copy resources (only if the res/ folder exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
        add_custom_command(TARGET ${SUBPROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/res
                $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
        )
endif()
