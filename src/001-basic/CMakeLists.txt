set(APP 001-basic)
add_executable(${APP} "")
target_sources(${APP} PRIVATE
  main.cpp
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

add_custom_command(
  TARGET ${APP}
  COMMENT "Copy assets"
  POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/resources
  $<TARGET_FILE_DIR:${APP}>/resources
  VERBATIM)
