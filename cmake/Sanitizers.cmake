# Enable sanitizers for target project_target
function(enable_sanitizers project_target)

  # Code coverage
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES
                                            "Clang")
    if(ENABLE_COVERAGE)
      target_compile_options(${project_target} INTERFACE --coverage -O0 -g)
      if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(${project_target} INTERFACE --coverage)
      else()
        target_link_libraries(${project_target} INTERFACE --coverage)
      endif()
    endif()

    # Start with an empty list of sanitizers
    set(_SANITIZERS "")

    # Address sanitizer
    option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    if(ENABLE_SANITIZER_ADDRESS)
      if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        list(APPEND _SANITIZERS "address")
      endif()
    endif()

    # Leak sanitizer
    option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    if(ENABLE_SANITIZER_LEAK)
      if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT APPLE)
        list(APPEND _SANITIZERS "leak")
      endif()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
       AND NOT ENABLE_SANITIZER_ADDRESS
       AND NOT ENABLE_SANITIZER_LEAK)
      # Memory sanitizer
      option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
      if(ENABLE_SANITIZER_MEMORY)
        if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND NOT APPLE)
          list(APPEND _SANITIZERS "memory")
        endif()
      endif()
    endif()

    # UB sanitizer
    option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
           "Enable undefined behavior sanitizer" OFF)
    if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
      if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        list(APPEND _SANITIZERS "undefined")
      endif()
    endif()

    # Thread sanitizer
    option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    if(ENABLE_SANITIZER_THREAD
       AND NOT ENABLE_SANITIZER_ADDRESS
       AND NOT ENABLE_SANITIZER_LEAK)
      if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        list(APPEND _SANITIZERS "thread")
      endif()
    endif()

    # Comma separated list of sanitizers
    list(JOIN _SANITIZERS "," LIST_OF_SANITIZERS)
  endif()

  # Set list of sanitizers if not empty
  if(LIST_OF_SANITIZERS)
    if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
      message(STATUS "List of sanitizers: ${LIST_OF_SANITIZERS}")
      if(ENABLE_SANITIZER_ADDRESS)
        target_compile_options(${project_target}
                               INTERFACE -fno-omit-frame-pointer)
      endif()
      target_compile_options(${project_target}
                             INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
      target_link_libraries(${project_target}
                            INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
    endif()
  endif()

endfunction()
