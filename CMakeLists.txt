cmake_minimum_required(VERSION 3.1)
project(Renderer)
set (CMAKE_CXX_STANDARD 11) # Use c++11

# Glob directory for all subdirectories with include subdirectory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child}/include)
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Add all deps/*/include directories to includes list
SUBDIRLIST(DEPDIRS ${CMAKE_CURRENT_LIST_DIR}/deps)
FOREACH(subdir ${DEPDIRS})
    message(STATUS "Adding " deps/${subdir}/include)
    set(DEPINCLUDES ${DEPINCLUDES} deps/${subdir}/include)
ENDFOREACH()

# nlohman::json has it's only header file in src...
set(DEPINCLUDES deps/json/src ${DEPINCLUDES})

set(INCLUDES include ./ ${DEPINCLUDES})

# Add all .cpp files from src to sources list
file(GLOB SOURCES "src/*.cpp")

# Add glad and stb_image to sources
set(SOURCES ${SOURCES} deps/glad/src/glad.c deps/stb_image/src/stb_image.c)

# Set includes and sources
include_directories(${INCLUDES})
set(SOURCES ${SOURCES})

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_CHDIR OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

# ASSIMP
set(ASSIMP_BUILD_ASSIMP_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "" FORCE)
set(ASSIMP_ENABLE_BOOST_WORKAROUND ON CACHE BOOL "" FORCE)
add_subdirectory(deps/assimp)

IF(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
ENDIF()

# Compile and link executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})
target_link_libraries(${PROJECT_NAME} glfw assimp)
