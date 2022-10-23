#include "window.hpp"

int main(int argc, char **argv) {
    try {
        abcg::Application app(argc, argv);

        Window window;
        window.setWindowSettings(
            {.width = 1000, .height = 1000, .title = "Paint"});

        app.run(window);
    } catch (std::exception const &exception) {
        fmt::print(stderr, "{}\n", exception.what());
        return -1;
    }
    return 0;
}
