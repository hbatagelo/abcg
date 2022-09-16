#include "window.hpp"

int main(int argc, char **argv) {
  try {
    // Create application instance
    abcg::Application app(argc, argv);

    // Create Vulkan window
    Window window;
    window.setWindowSettings(
        {.width = 600, .height = 600, .title = "Hello, World!"});

    // Run application
    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}", exception.what());
    return -1;
  }
  return 0;
}