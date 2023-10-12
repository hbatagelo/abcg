#include "window.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings({.samples = 2, .doubleBuffering = false});
    window.setWindowSettings({
        .width = 600,
        .height = 600,
        .title = "Regular Polygons",
    });

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}