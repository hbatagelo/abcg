function(enable_abcg project_target)

  if(ARGC GREATER 1)
    # If a second argument is passed, it is treated as a string containing extra
    # libraries to be linked with
    target_link_libraries(${project_target} PUBLIC abcg ${ARGV1})
  else()
    target_link_libraries(${project_target} PUBLIC abcg)
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows"
     AND NOT ENABLE_CONAN
     AND NOT MSVC)
    if(${GRAPHICS_API} MATCHES "OpenGL")
      target_link_libraries(${project_target} PUBLIC -lmingw32 -lSDL2main
                                                     -lSDL2 -lglew32)
    elseif(${GRAPHICS_API} MATCHES "Vulkan")
      target_link_libraries(${project_target} PUBLIC -lmingw32 -lSDL2main
                                                     -lSDL2 -lvulkan)
    else()
      target_link_libraries(${project_target} PUBLIC -lmingw32 -lSDL2main
                                                     -lSDL2)
    endif()
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
    list(APPEND LINK_FLAGS "-sSTACK_SIZE=1MB")
    list(APPEND LINK_FLAGS "--use-preload-plugins")
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
      list(APPEND LINK_FLAGS
           "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets ")
    endif()
    string(REPLACE ";" " " LINK_FLAGS "${LINK_FLAGS}")

    set_target_properties(
      ${project_target}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/public"
                 LINK_FLAGS "${LINK_FLAGS}")
  else()
    target_compile_features(${project_target} PUBLIC cxx_std_20)

    if(NOT MSVC)
      target_compile_options(${project_target} PUBLIC -Wall -Wextra -pedantic)
    endif()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set_target_properties(
      ${project_target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    get_target_property(output_dir ${project_target} RUNTIME_OUTPUT_DIRECTORY)

    # Building from Visual Studio IDE
    if(MSVC AND ${output_dir} MATCHES "/out/build/")
      # Copy assets directory to ${output_dir}
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
        add_custom_command(
          TARGET ${project_target}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_CURRENT_SOURCE_DIR}/assets ${output_dir}/assets)
      endif()

      # Copy DLLs of SDL2 Extract first string delimited by ';', extract path
      # then copy
      list(GET SDL2_LIBRARY 0 SDL2_FIRSTPATH)
      string(REGEX REPLACE "(.+\/).+\.lib" "\\1" SDL2_LIBPATH ${SDL2_FIRSTPATH})
      file(GLOB SDL2_DLLS "${SDL2_LIBPATH}*.dll")
      file(COPY ${SDL2_DLLS} DESTINATION ${output_dir})

      # Copy DLLs of SDL2_image
      list(GET SDL2_IMAGE_LIBRARIES 0 SDL2_IMG_FIRSTPATH)
      string(REGEX REPLACE "(.+\/).+\.lib" "\\1" SDL2_IMG_LIBPATH
                           ${SDL2_IMG_FIRSTPATH})
      file(GLOB SDL2_IMG_DLLS "${SDL2_IMG_LIBPATH}*.dll")
      file(COPY ${SDL2_IMG_DLLS} DESTINATION ${output_dir})
    else()
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

      # Make directory ${project_target}.dir
      add_custom_command(
        TARGET ${project_target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${output_dir}/${project_target}.dir)

      # Copy assets directory to ${project_target}.dir
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
        add_custom_command(
          TARGET ${project_target}
          POST_BUILD
          COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            ${output_dir}/${project_target}.dir/assets)
      endif()

      # Take into account that, on Windows with MSVC, binaries are placed in a
      # subdirectory named after the build type
      set(build_type "")
      if(MSVC)
        string(
          APPEND build_type "$<$<CONFIG:Debug>:Debug/>"
          "$<$<CONFIG:Release>:Release/>" "$<$<CONFIG:MinSizeRel>:MinSizeRel/>"
          "$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo/>")

        # Copy DLLs of SDL2 Extract first string delimited by ';', extract path
        # then copy
        list(GET SDL2_LIBRARY 0 SDL2_FIRSTPATH)
        string(REGEX REPLACE "(.+\/).+\.lib" "\\1" SDL2_LIBPATH
                             ${SDL2_FIRSTPATH})
        file(GLOB SDL2_DLLS "${SDL2_LIBPATH}*.dll")
        file(COPY ${SDL2_DLLS} DESTINATION ${output_dir}/${project_target}.dir/)

        # Copy DLLs of SDL2_image
        list(GET SDL2_IMAGE_LIBRARIES 0 SDL2_IMG_FIRSTPATH)
        string(REGEX REPLACE "(.+\/).+\.lib" "\\1" SDL2_IMG_LIBPATH
                             ${SDL2_IMG_FIRSTPATH})
        file(GLOB SDL2_IMG_DLLS "${SDL2_IMG_LIBPATH}*.dll")
        file(COPY ${SDL2_IMG_DLLS}
             DESTINATION ${output_dir}/${project_target}.dir/)
      endif()

      add_custom_command(
        TARGET ${project_target}
        POST_BUILD
        COMMAND
          # Copy executable from output directory to ${project_target}.dir
          ${CMAKE_COMMAND} -E copy
          ${output_dir}/${build_type}${project_target}${extension}
          ${output_dir}/${project_target}.dir/${project_target}${extension}
        COMMAND # Remove executable from output directory
                ${CMAKE_COMMAND} -E remove
                ${output_dir}/${build_type}${project_target}${extension}
        COMMAND_EXPAND_LISTS)

      add_custom_command(
        TARGET ${project_target}
        POST_BUILD
        COMMAND # Rename ${project_target}.dir to ${project_target}
                ${CMAKE_COMMAND} -E rename ${output_dir}/${project_target}.dir
                ${output_dir}/${project_target}
        COMMAND # Remove ${project_target}.dir (useful if renaming has failed)
                ${CMAKE_COMMAND} -E rm -rf ${output_dir}/${project_target}.dir)
    endif()

  endif()

endfunction()
