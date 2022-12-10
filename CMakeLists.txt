cmake_minimum_required(VERSION 3.16)
project(B-Spline-curve-fitting)

# C++17 is required
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

# turn off compiler warnings
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
endif()

# setup GLFW CMake project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw)

# add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include/GLFW)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/tinynurbs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/glm/glm)

# add source files
file(GLOB_RECURSE PROJECT_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h*)
file(GLOB_RECURSE PROJECT_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE IMGUI_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/include/*.h)
file(GLOB_RECURSE TINYNURBS_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/external/tinynurbs/*.h*)
file(GLOB_RECURSE IMGUI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/src/*.cpp)
file(GLOB_RECURSE GLAD_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/*.c)

# group files in IDE
source_group("src" FILES ${PROJECT_SRC_FILES})
source_group("include" FILES ${PROJECT_HEADER_FILES})
source_group("external\\imgui" FILES ${IMGUI_SRC_FILES})
source_group("external\\glad" FILES ${GLAD_SRC_FILES} )
source_group("external\\tinynurbs" FILES ${TINYNURBS_HEADER_FILES} )

#create executable 
add_executable( ${PROJECT_NAME} 
                    ${PROJECT_SRC_FILES}
                    ${PROJECT_HEADER_FILES}
                    ${IMGUI_HEADER_FILES}
                    ${IMGUI_SRC_FILES}
                    ${TINYNURBS_HEADER_FILES}
                    ${GLAD_SRC_FILES})

# link libraries
target_link_libraries(${PROJECT_NAME} glfw)