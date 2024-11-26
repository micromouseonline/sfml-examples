include(${CMAKE_SOURCE_DIR}/cmake/project-boilerplate.cmake)

target_sources(${APP} PRIVATE
        main.cpp
        window.cpp
        application.cpp
        robot.cpp
        robot-control.cpp
        mazedata.cpp
)
