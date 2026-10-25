cmake_minimum_required(VERSION 3.15)
project(LeapEngine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE LEAP_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.c")
add_library(LeapEngine STATIC ${LEAP_SOURCES})
add_library(LeapEngine::LeapEngine ALIAS LeapEngine)

target_compile_features(LeapEngine PUBLIC cxx_std_17)
target_include_directories(LeapEngine
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/include)
target_link_libraries(LeapEngine PRIVATE glfw OpenGL::GL Threads::Threads ${CMAKE_DL_LIBS})
if(UNIX AND NOT APPLE)
    target_link_libraries(LeapEngine PRIVATE m)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(LeapDemo examples/demo/main.cpp)
    target_link_libraries(LeapDemo PRIVATE LeapEngine::LeapEngine)
endif()