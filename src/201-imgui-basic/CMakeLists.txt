set(APP 201-imgui-basic)
add_executable(${APP} "")
target_sources(${APP} PRIVATE
        main.cpp
)
target_compile_options(${APP} PRIVATE
        ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
)

add_compiler_info(${APP})
#add_data_file_copy(${APP})

target_link_libraries(
        ${APP} PRIVATE
        ${SANITIZER_FLAGS}
        ${SFML_LIBS}
        ${IMGUI_LIBS}
)
