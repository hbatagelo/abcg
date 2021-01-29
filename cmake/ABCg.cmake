function(enable_abcg project_target)

  target_link_libraries(${project_target} PUBLIC abcg)

  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT ENABLE_CONAN)
    target_link_libraries(${project_target} PUBLIC -lmingw32 -lSDL2main -lSDL2
                                                   -lglew32)
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_compile_options(${project_target} PUBLIC -Wall -Wextra -pedantic)
    target_compile_options(
      ${project_target}
      PUBLIC "-std=c++20"
      PUBLIC "-Oz"
      PUBLIC "-sUSE_SDL=2"
      PUBLIC "-sUSE_SDL_IMAGE=2")

    set(LINK_FLAGS "")
    # For debugging, use ASSERTIONS=1 and DISABLE_EXCEPTION_CATCHING=0
    list(APPEND LINK_FLAGS "-sASSERTIONS=0")
    list(APPEND LINK_FLAGS "-sDISABLE_EXCEPTION_CATCHING=1")
    list(APPEND LINK_FLAGS "-sALLOW_MEMORY_GROWTH=1")
    list(APPEND LINK_FLAGS "-sEXIT_RUNTIME=1")
    list(APPEND LINK_FLAGS "-sFULL_ES3=1")
    list(APPEND LINK_FLAGS "-sMAX_WEBGL_VERSION=2")
    list(APPEND LINK_FLAGS "-sMIN_WEBGL_VERSION=2")
    list(APPEND LINK_FLAGS "-sUSE_SDL=2")
    list(APPEND LINK_FLAGS "-sUSE_SDL_IMAGE=2")
    list(APPEND LINK_FLAGS "-sWASM=1")
    list(APPEND LINK_FLAGS "--use-preload-plugins")
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
      list(APPEND LINK_FLAGS
           "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
    endif()
    string(REPLACE ";" " " LINK_FLAGS "${LINK_FLAGS}")

    set_target_properties(
      ${project_target}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/public"
                 LINK_FLAGS "${LINK_FLAGS}")
  else()
    target_compile_features(${project_target} PUBLIC cxx_std_20)
    target_compile_options(${project_target} PUBLIC -Wall -Wextra -pedantic)

    if(NOT ENABLE_CONAN)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
      set_target_properties(
        ${project_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                     "${CMAKE_BINARY_DIR}/bin")
    endif()

    get_target_property(output_dir ${project_target} RUNTIME_OUTPUT_DIRECTORY)

    # PRE_BUILD: clean up build directory
    add_custom_command(
      TARGET ${project_target}
      PRE_BUILD
      COMMAND # Remove directory ${project_target}
              ${CMAKE_COMMAND} -E remove_directory
              ${output_dir}/${project_target})

    # POST_BUILD: copy executable and assets to bin directory
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
      set(extension ".exe")
    endif()

    add_custom_command(
      TARGET ${project_target}
      POST_BUILD
      COMMAND # Make directory ${project_target}.dir
              ${CMAKE_COMMAND} -E make_directory
              ${output_dir}/${project_target}.dir
      COMMAND
        # Copy executable from output directory to ${project_target}.dir
        ${CMAKE_COMMAND} -E copy ${output_dir}/${project_target}${extension}
        ${output_dir}/${project_target}.dir/${project_target}${extension}
      COMMAND # Remove executable from output directory
              ${CMAKE_COMMAND} -E remove
              ${output_dir}/${project_target}${extension})

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
      add_custom_command(
        TARGET ${project_target}
        POST_BUILD
        COMMAND
          # Copy assets directory to ${project_target}.dir
          ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
          ${output_dir}/${project_target}.dir/assets)
    endif()

    add_custom_command(
      TARGET ${project_target}
      POST_BUILD
      COMMAND # Rename ${project_target}.dir to ${project_target}
              ${CMAKE_COMMAND} -E rename ${output_dir}/${project_target}.dir
              ${output_dir}/${project_target})

  endif()

endfunction()
