cmake_minimum_required(VERSION 3.20)
project(pag_practica1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#cosas de glfw
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# cosas de glad
file(GLOB MY_FILES *.cpp)
file(GLOB GLAD_FILES GLAD/src/*.c)

add_executable(${PROJECT_NAME} ${MY_FILES} ${GLAD_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC GLAD/include)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)