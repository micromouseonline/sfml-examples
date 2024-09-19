cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project("ImGui-SFML-Starter" LANGUAGES CXX)

# Set project directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

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

include_directories(${imgui_SOURCE_DIR})
include_directories(libs/implot)

add_subdirectory(libs/implot)
set(IMPLOT_LIBS PUBLIC implot)

set(SFML_LIBS PUBLIC sfml-graphics sfml-system sfml-window)
set(IMGUI_LIBS PUBLIC ImGui-SFML::ImGui-SFML)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src/001-basic-window-and-shapes)
add_subdirectory(src/002-shapes-and-textures)
add_subdirectory(src/002b-sprites)
add_subdirectory(src/002c-sprite-animated)
add_subdirectory(src/003-text-and-fonts)
add_subdirectory(src/004-events)
add_subdirectory(src/005-mouse)
add_subdirectory(src/006-keyboard)
add_subdirectory(src/007-vertex-arrays)
add_subdirectory(src/008-tilemap)
add_subdirectory(src/201-imgui-basic)
add_subdirectory(src/301-implot-demo)
add_subdirectory(src/708-tilemap)
add_subdirectory(src/808-wallmap)
add_subdirectory(src/009-view-map)
add_subdirectory(src/009-view-rendertexture)
add_subdirectory(src/009-views)
add_subdirectory(src/010-render-to-bitmap)
add_subdirectory(src/011-raycast-sensors)

# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
# endif()

# Define the directory containing your asset files
set(DATA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)

# Define the directory where you want to copy the data files
set(DATA_DEST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

# Copy data files after building the target
add_custom_target(copy_data_files ALL
        COMMENT "Copying data files to binary directory"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DATA_DEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SOURCE_DIR} ${DATA_DEST_DIR}
        DEPENDS ${DATA_SOURCE_DIR}
)

# Set a dependency on copy_data_files for each of your target executables
foreach (TARGET_EXECUTABLE ${TARGET_EXECUTABLES})
    add_dependencies(${TARGET_EXECUTABLE} copy_data_files)
endforeach ()
