set(APP 808-wallmap)
add_executable(${APP} "")
target_sources(${APP} PRIVATE
        main.cpp
        japan2007ef.cpp
)
target_compile_options(${APP} PRIVATE
        ${DEFAULT_COMPILER_OPTIONS_AND_WARNINGS}
)
target_link_libraries(
        ${APP} PRIVATE
        ${SANITIZER_FLAGS}
        ${SFML_LIBS}
        ${IMGUI_LIBS}
)
