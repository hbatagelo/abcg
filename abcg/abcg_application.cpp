/**
 * @file abcg_application.cpp
 * @brief Definition of abcg::Application class members.
 *
 * This project is released under the MIT License.
 */

#define TINYOBJLOADER_IMPLEMENTATION
#include "abcg_application.hpp"

#include <fmt/core.h>

#include <span>

#include "SDL_image.h"
#include "abcg_exception.hpp"
#include "abcg_openglwindow.hpp"
#include "tiny_obj_loader.h"

#if defined(__EMSCRIPTEN__)
void abcg::mainLoopCallback(void *userData) {
  abcg::Application &app{*(static_cast<abcg::Application *>(userData))};
  bool done{};
  app.mainLoopIterator(done);
}
#endif

/**
 * @brief Constructs an abcg::Application object.
 *
 * Constructs an abcg::Application object and initializes SDL library and
 * subsystems.
 *
 * @throw abcg::Exception if SDL failed to initialize its subsystems.
 */
abcg::Application::Application([[maybe_unused]] int argc, char **argv) {
  Uint32 subsystemMask{SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO |
                       SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
                       SDL_INIT_EVENTS};

  if (SDL_Init(subsystemMask) != 0) {
    throw abcg::Exception{abcg::Exception::SDL("SDL_Init failed")};
  }

#if !defined(__EMSCRIPTEN__)
  // Load support for the PNG image format
  auto imageFlags{IMG_INIT_PNG};
  if (auto initialized{IMG_Init(imageFlags)};
      (initialized & imageFlags) != imageFlags) {
    throw abcg::Exception{abcg::Exception::SDLImage("IMG_Init failed")};
  }
#endif

  // Get executable relative path
  std::string argv_str(*std::span{&argv, 1}[0]);
#if defined(WIN32)
  if (auto n{argv_str.find_last_of('\\')}; n == std::string::npos) {
    // Called from the same directory of the executable
    m_basePath = ".";
  } else {
    m_basePath = argv_str.substr(0, n);
  }
#else
  m_basePath = argv_str.substr(0, argv_str.find_last_of('/'));
#endif
}

/**
 * @brief Destroys the abcg::Application object.
 *
 * Destroys the abcg::Application object and cleans up SDL initialized
 * subsystems.
 */
abcg::Application::~Application() {
#if !defined(__EMSCRIPTEN__)
  IMG_Quit();
#endif
  SDL_Quit();
}

/**
 * @brief Runs the application for a single window.
 *
 * @param window Unique pointer to window.
 *
 * @throw abcg::Exception if window is a null pointer.
 */
void abcg::Application::run(std::unique_ptr<OpenGLWindow> window) {
  if (window != nullptr) {
    m_window = std::move(window);
    run();
  } else {
    throw abcg::Exception{abcg::Exception::Runtime("Null pointer")};
  }
}

void abcg::Application::mainLoopIterator([[maybe_unused]] bool &done) {
  SDL_Event event{};
  while (SDL_PollEvent(&event) != 0) {
#if !defined(__EMSCRIPTEN__)
    if (event.type == SDL_QUIT) done = true;
#endif
    m_window->handleEvent(event, done);
  }
  m_window->paint();
}

void abcg::Application::run() {
  m_window->initialize(m_basePath);

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(mainLoopCallback, this, 0, true);
#else
  bool done{};
  while (!done) {
    mainLoopIterator(done);
  };
#endif
}
