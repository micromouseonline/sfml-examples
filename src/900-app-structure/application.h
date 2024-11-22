//
// Created by peter on 22/11/24.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include "window.h"

class Application {
 public:
  Application();
  void Run();

  void processEvents();
  void Update(sf::Time deltaTime = sf::seconds(0.01));
  void Render();

  void HandleInput();
  Window* GetWindow();
  sf::Time GetElapsed();
  void RestartClock();
  void UpdateStatistics(sf::Time elapsedTime);
  void HandleUserInput(sf::Keyboard::Key key, bool isPressed);

 private:
  static const sf::Time TimePerFrame;
  Window m_window;
  sf::Clock m_clock;
  sf::Time m_elapsed;

  sf::Font mFont;
  sf::Text mStatisticsText;
  sf::Time mStatisticsUpdateTime;

  std::size_t mStatisticsNumFrames;

  void MoveMushroom();
  sf::Texture m_mushroomTexture;
  sf::Sprite m_mushroom;
  sf::Vector2i m_increment;
};

#endif  // APPLICATION_H
