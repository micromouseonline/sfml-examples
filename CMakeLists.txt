cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project("sfml-tutorial" LANGUAGES CXX)

# Set project directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# default compiler options and warnings
include(${CMAKE_SOURCE_DIR}/cmake/DefaultCompilerOptionsAndWarnings.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/print_compiler_info.cmake)

macro(print_compiler_info)
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endmacro()

# Define the add_compiler_info function
function(add_compiler_info target)
    add_custom_command(
            TARGET ${target} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}"
            COMMAND ${CMAKE_COMMAND} -E echo "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}"
    )
endfunction()

function(add_data_file_copy target)
    add_custom_command(
            TARGET ${target} POST_BUILD
            COMMENT "Copying data files to binary directory"
            COMMAND ${CMAKE_COMMAND} -E make_directory ${DATA_DEST_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SOURCE_DIR} ${DATA_DEST_DIR}
            DEPENDS ${DATA_SOURCE_DIR}
    )
endfunction()

# static linking for easy installation
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

# ###############################
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# ###############################
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

# ###############################
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.0.1 # You can specify the version you need
)
FetchContent_MakeAvailable(fmt)

# ###############################
set(SFML_LIBS sfml-graphics sfml-system sfml-window)
set(IMGUI_LIBS ImGui-SFML::ImGui-SFML)
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
add_subdirectory(src/012-raycast-collision-detection)

# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
# endif()

# if (WIN32)
# add_custom_command(
# TARGET ${PROJECT_NAME}
# COMMENT "Copy OpenAL DLL"
# PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
# VERBATIM)
# endif ()

# ############################### be sure to copy any assets into the binary directory

# Define the directory containing your asset files
set(DATA_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)

# Define the directory where you want to copy the data files
set(DATA_DEST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)


add_custom_target(show_compiler_info ALL
        COMMAND ${CMAKE_COMMAND} -E echo "C++ Compiler ID: ${CMAKE_CXX_COMPILER_ID}"
        COMMAND ${CMAKE_COMMAND} -E echo "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}"
)

# Copy data files after building the target
add_custom_target(copy_data_files ALL
        COMMENT "Copying data files to binary directory"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DATA_DEST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SOURCE_DIR} ${DATA_DEST_DIR}
        DEPENDS ${DATA_SOURCE_DIR}
)

# Set a dependency on copy_data_files for each of your target executables
foreach (TARGET_EXECUTABLE ${TARGET_EXECUTABLES})
    message(STATUS "harry")
    add_dependencies(${TARGET_EXECUTABLE} copy_data_files)
endforeach ()

