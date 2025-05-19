cmake_minimum_required(VERSION 3.10)

project(OpenGLSurfaceProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_library(MyMath
        src/MyMath/vec3.cpp
        src/MyMath/mat4.cpp
)

target_include_directories(MyMath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(OpenGLSurfaceApp
            src/main.cpp
            src/Shader.cpp
            src/Camera.cpp
            src/PointSet.cpp
            src/Curve.cpp
            src/RevolutionSurface.cpp
    )

target_include_directories(OpenGLSurfaceApp PUBLIC include)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_link_libraries(OpenGLSurfaceApp PRIVATE
        MyMath
        OpenGL::GL
        GLEW::GLEW
        glfw
)
