#include "window.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>

int main(int argc, char **argv) {
    std::srand(std::time(nullptr));
    try {
        abcg::Application app(argc, argv);

        Window window;
        window.setWindowSettings(
            {.width = 800, .height = 800, .title = "Boid"});

        app.run(window);
    } catch (std::exception const &exception) {
        fmt::print(stderr, "{}\n", exception.what());
        return -1;
    }
    return 0;
}
