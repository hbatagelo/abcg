project(helicopter)
add_executable(${PROJECT_NAME} main.cpp model.cpp window.cpp trackball.cpp)
enable_abcg(${PROJECT_NAME})
