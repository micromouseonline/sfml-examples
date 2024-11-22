//
// Created by peter on 22/11/24.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <SFML/Graphics.hpp>
#include <string>

class Window {
 public:
  Window();
  Window(const std::string& title, const sf::Vector2u& size);
  ~Window();

  void BeginDraw();
  void EndDraw();

  void Update();

  bool IsDone() const;
  bool IsFullscreen() const;
  sf::Vector2u GetWindowSize();

  void ToggleFullscreen();

  void Draw(sf::Drawable& l_drawable);

 private:
  void Setup(const std::string title, const sf::Vector2u& size);
  void Create();
  void Destroy();

  sf::RenderWindow m_window;
  sf::Vector2u m_windowSize;
  std::string m_windowTitle;
  bool m_isDone;
  bool m_isFullscreen;
};

#endif  // IMGUI_SFML_STARTER_WINDOW_H
