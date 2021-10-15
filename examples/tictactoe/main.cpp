#include <fmt/core.h>
#include "abcg.hpp"
#include "openglwindow.hpp"

int main(int argc, char **argv) {
    try {
        // Criando a instancia da aplicaçao
        abcg::Application app(argc, argv); 

        // Criando a tela OpenGL
        auto window{std::make_unique<OpenGLWindow>()};
        window->setWindowSettings({.title = "First App"});

        // rodar aplicaçao
        app.run(std::move(window));
    } catch (const abcg::Exception &exception) {
        fmt::print(stderr, "{}\n", exception.what());
        return -1;
    }
    return 0;
}