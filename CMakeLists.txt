cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project("ImGui-SFML-Starter" LANGUAGES CXX)

# Set project directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE})

# default compiler options and warnings
include(cmake/DefaultCompilerOptionsAndWarnings.cmake)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9-docking)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

set(SFML_LIBS sfml-graphics sfml-system sfml-window)
set(IMGUI_LIBS ImGui-SFML::ImGui-SFML)

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/001-basic)
add_subdirectory(src/002-basic)

# add_executable(001-app "")
# target_sources(001-app PRIVATE
# src/001-main.cpp
# )
# target_compile_options(001-app PRIVATE
# ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
# )
# target_link_libraries(
# 001-app PRIVATE
# ${SANITIZER_FLAGS}
# ${SFML_LIBS}
# ${IMGUI_LIBS}
# )

# target_compile_features(001-app PUBLIC cxx_std_20)
# add_executable(002-app "")
# target_sources(002-app PRIVATE
# src/002-main.cpp
# )
# target_compile_options(002-app PRIVATE
# ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
# )
# target_link_libraries(
# 002-app PRIVATE
# ${SANITIZER_FLAGS}
# ${SFML_LIBS}
# ${IMGUI_LIBS}
# )

# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
# endif()
# add_custom_command(
# TARGET 001-app
# COMMENT "Copy assets"
# POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
# ${CMAKE_SOURCE_DIR}/resources
# $<TARGET_FILE_DIR:001-app>/resources
# VERBATIM)
