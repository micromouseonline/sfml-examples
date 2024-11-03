set(APP 302-implot-with-imgui)
add_executable(${APP} "")
target_sources(${APP} PRIVATE
        main.cpp
        implot_demo.cpp
)
target_compile_options(${APP} PRIVATE
        ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
)

target_link_libraries(
        ${APP} PRIVATE
        ${SANITIZER_FLAGS}
        ${SFML_LIBS}
        ${IMGUI_LIBS}
        ${IMPLOT_LIBS}
)
