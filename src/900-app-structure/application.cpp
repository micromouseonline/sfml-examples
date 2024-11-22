//
// Created by peter on 22/11/24.
//

#include "application.h"
#include <string-helper.h>
#include <SFML/Graphics.hpp>

Application::Application() : m_window("Chapter 2", sf::Vector2u(800, 600)) {
  RestartClock();
  m_mushroomTexture.loadFromFile("assets/images/mouse-b.png");
  m_mushroom.setTexture(m_mushroomTexture);
  m_mushroom.setPosition(100, 100);
  m_increment = sf::Vector2i(400, 400);
}

void Application::Run() {
}

void Application::processEvents() {
}

void Application::Update(sf::Time deltaTime) {
  m_window.Update();  // must we do this first? It grabs all the events
  MoveMushroom();
}

void Application::Render() {
  m_window.BeginDraw();
  m_window.Draw(m_mushroom);
  m_window.EndDraw();
}

void Application::MoveMushroom() {
}

void Application::UpdateStatistics(sf::Time elapsedTime) {
}

void Application::HandleUserInput(sf::Keyboard::Key key, bool isPressed) {
}
void Application::HandleInput() {
}
void Application::RestartClock() {
  m_elapsed = m_clock.restart();
}
sf::Time Application::GetElapsed() {
  return m_elapsed;
}
Window* Application::GetWindow() {
  return &m_window;
}
