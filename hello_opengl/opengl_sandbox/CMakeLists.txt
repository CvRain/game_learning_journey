cmake_minimum_required(VERSION 3.28)

set(SUBPROJECT_NAME "opengl_sandbox")

project(${SUBPROJECT_NAME} CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找必要的包
find_package(glad CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# 包含头文件目录
include_directories(include)

# 自动收集源文件
file(GLOB_RECURSE CPP_FILES src/*.cpp)
file(GLOB_RECURSE H_FILES src/*.h src/*.hpp)

add_executable(${SUBPROJECT_NAME}
        ${CPP_FILES}
        ${H_FILES}
)

# 链接库及依赖
target_link_libraries(${SUBPROJECT_NAME} PRIVATE
        glad::glad
        SDL3::SDL3
        glm::glm
)

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
