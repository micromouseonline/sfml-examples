get_filename_component(CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(APP ${CURRENT_DIR_NAME})
add_definitions(-DWINDOW_TITLE=\"${APP}\")

add_executable(${APP} "")
target_sources(${APP} PRIVATE
        main.cpp
)
target_compile_options(${APP} PRIVATE
        ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
)

copy_resources(${APP})
copy_windows_dlls(${APP})

target_link_libraries(
        ${APP} PRIVATE
        ${SANITIZER_FLAGS}
        ${SFML_LIBS}
        ${IMGUI_LIBS}
)
