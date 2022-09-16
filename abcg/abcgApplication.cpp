/**
 * @file abcgApplication.cpp
 * @brief Definition of abcg::Application members.
 *
 * This file is part of ABCg (https://github.com/hbatagelo/abcg).
 *
 * @copyright (c) 2021--2022 Harlen Batagelo. All rights reserved.
 * This project is released under the MIT License.
 */

#include "abcgApplication.hpp"

#include <SDL_image.h>
#include <SDL_thread.h>

#include <span>

#include "abcgException.hpp"
#include "abcgWindow.hpp"

#if defined(__EMSCRIPTEN__)
#include "abcgOpenGLExternal.hpp"
#endif

// @cond Skipped by Doxygen
#define TINYOBJLOADER_IMPLEMENTATION
// @endcond
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
 * @param argc Number of arguments passed to the program from the environment in
 * which the program is run.
 * @param argv Pointer to the first element of an array of @a argc + 1 pointers,
 * of which the last one is nullptr and the previous ones, if any, point to
 * null-terminated multibyte strings that represent the arguments passed to the
 * program from the execution environment.
 */
abcg::Application::Application([[maybe_unused]] int argc, char **argv) {
  // Get executable relative path
  std::string const argv_str{*std::span{&argv, 1}[0]};
#if defined(WIN32)
  if (auto const pos{argv_str.find_last_of('\\')}; pos == std::string::npos) {
    // Called from the same directory of the executable
    abcg::Application::m_basePath = ".";
  } else {
    abcg::Application::m_basePath = argv_str.substr(0, pos);
  }
#else
  abcg::Application::m_basePath =
      argv_str.substr(0, argv_str.find_last_of('/'));
#endif

  abcg::Application::m_assetsPath = abcg::Application::m_basePath + "/assets/";
}

/**
 * @brief Runs the application for the given window.
 *
 * Initializes the SDL library and its subsystems, initializes the window and
 * runs the event loop.
 *
 * @param window L-value reference to the window object.
 *
 * @throw abcg::SDLError if `SDL_Init` failed.
 * @throw abcg::SDLImageError if `IMG_Init` failed.
 */
void abcg::Application::run(Window &window) {
  if (Uint32 const subsystemMask{SDL_INIT_VIDEO | SDL_INIT_AUDIO |
                                 SDL_INIT_GAMECONTROLLER};
      SDL_Init(subsystemMask) != 0) {
    throw abcg::SDLError("SDL_Init failed");
  }

#if !defined(__EMSCRIPTEN__)
  // Load support for JPEG and PNG image formats
  auto const imageFlags{IMG_INIT_JPG | IMG_INIT_PNG};
  if (auto const initialized{IMG_Init(imageFlags)};
      (initialized & imageFlags) != imageFlags) {
    throw abcg::SDLImageError("IMG_Init failed");
  }
#endif

  m_window = &window;
  m_window->templateCreate();

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(mainLoopCallback, this, 0, true);
#else
  auto done{false};
  while (!done) {
    mainLoopIterator(done);
  };
#endif

  m_window->templateDestroy();

#if !defined(__EMSCRIPTEN__)
  IMG_Quit();
#endif
  SDL_Quit();
}

void abcg::Application::mainLoopIterator([[maybe_unused]] bool &done) const {
  SDL_Event event{};
  while (SDL_PollEvent(&event) != 0) {
#if !defined(__EMSCRIPTEN__)
    if (event.type == SDL_QUIT)
      done = true;
#endif
    m_window->templateHandleEvent(event, done);
  }
  m_window->templatePaint();
}
