//
// Created by peter on 22/11/24.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <SFML/Graphics.hpp>
#include <string>

/***
 * Implements a Window class that wraps sf::RenderWindow to provide a clean interface
 * and features including window creation and destruction, window updates, window drawing
 * and window closing
 */
class Window {
 public:
  Window();
  Window(const std::string& title, const sf::Vector2u& size);
  ~Window();

  void SetTitle(const std::string& title);

  /// Clears the window background
  void BeginDraw();

  /// Displays the changes made to the window
  void EndDraw();

  /// This seems to suck up the events. needs more work
  void Update();

  /// The normal close events merely set a flag that can be tested with this getter.
  /// That allows the application to do some cleanup before exiting.
  bool IsDone() const;

  void ToggleFullscreen();
  bool IsFullscreen() const;

  sf::Vector2u GetWindowSize();

  /// Used to draw any drawable object into the window. The actual window variable
  /// is private so we do not have direct access to it
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
