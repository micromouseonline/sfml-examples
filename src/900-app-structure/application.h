//
// Created by peter on 22/11/24.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include "window.h"

#include "drawing.h"
#include "maze.h"
#include "robot-control.h"
#include "robot-display.h"
#include "robot.h"
#include "sensor-data.h"

class Application {
 public:
  Application() : m_window("Application", sf::Vector2u(800, 600)) { RestartClock(); };

  void Run() {
    while (!GetWindow()->IsDone()) {
      // Application loop.
      ProcessEvents();
      Update();
      Render();
      RestartClock();
    }
  };

  void ProcessEvents() {

  };

  void Update(sf::Time deltaTime = sf::seconds(0.01)) { m_window.Update(); };

  void Render() {
    m_window.BeginDraw();
    sf::RenderWindow& window = *m_window.GetRenderWindow();

    draw_line(window, {40, 90}, {100, 100}, sf::Color::Red);
    m_window.EndDraw();
  }

  Window* GetWindow() { return &m_window; }
  sf::Time GetElapsed() { return m_elapsed; }

  void RestartClock() { m_elapsed = m_clock.restart(); }

  void UpdateStatistics(sf::Time elapsedTime) {};
  void HandleUserInput(sf::Keyboard::Key key, bool isPressed) {};

 private:
  static const sf::Time TimePerFrame;
  Window m_window;
  sf::Clock m_clock;
  sf::Time m_elapsed;

  sf::Font mFont;
  sf::Text mStatisticsText;
  sf::Time mStatisticsUpdateTime;

  std::size_t mStatisticsNumFrames;
};

#endif  // APPLICATION_H
