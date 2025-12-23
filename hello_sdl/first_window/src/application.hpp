//
// Created by cvrain on 2025/12/23.
//

#ifndef GAMELEARNINGJOURNEY_APPLICATION_HPP
#define GAMELEARNINGJOURNEY_APPLICATION_HPP

#include <SDL3/SDL.h>
#include <memory>
#include <string_view>

class Application {
public:
  bool init();

  // SDL3 callback 模式：返回 SDL_AppResult
  SDL_AppResult update();

  void handleEvents(const SDL_Event *event);

  ~Application();

private:
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  const std::string_view window_title = "Hello SDL first window";
  const std::string_view renderer_title = "Hello SDL renderer";
  const int window_width = 640;
  const int window_height = 480;
};

#endif // GAMELEARNINGJOURNEY_APPLICATION_HPP
