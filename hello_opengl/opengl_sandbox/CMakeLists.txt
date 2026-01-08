set(SUBPROJECT_NAME "opengl_sandbox")

# 设置C++标准
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(include)

# 添加源文件
set(CPP_FILES
        src/main.cpp
)

set(CPP_MODULES
)

set(SHADER_FILES
)

add_executable(${SUBPROJECT_NAME}
        ${CPP_FILES}
        ${CPP_MODULES}
        ${SHADER_FILES}
)


# 链接库及依赖
target_link_libraries(${SUBPROJECT_NAME} PRIVATE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    add_custom_command(TARGET ${SUBPROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:${SUBPROJECT_NAME}>/res
            COMMENT "Copying resources"
    )
endif()

# 设置输出目录
set_target_properties(${SUBPROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SUBPROJECT_NAME}"
)
