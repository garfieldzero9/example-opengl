cmake_minimum_required(VERSION 3.10)
project(example_opengl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0072 OLD)

# Find Packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

# Use PkgConfig to find the dependencies
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(CGLM REQUIRED cglm)

add_executable(app src/main.c src/shader_utils.c src/gl_ext.c)

target_include_directories(app PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${CGLM_INCLUDE_DIRS}
)

target_link_libraries(app PUBLIC
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${CGLM_LIBRARIES}
    m
)