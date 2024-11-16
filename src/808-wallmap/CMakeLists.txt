set(APP 808-wallmap)
add_executable(${APP} ""
        walls.h
        mazedata.h
        robotview.h
        robot.cpp
        robot.h)
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
