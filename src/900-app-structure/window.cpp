//
// Created by peter on 22/11/24.
//

#include "window.h"

Window::Window() {
  m_window.create(sf::VideoMode(640, 480), "Window from constructor");
}

Window::Window(const std::string& title, const sf::Vector2u& size) {
  Setup(title, size);
}

Window::~Window() {
  Destroy();
}

void Window::Setup(const std::string title, const sf::Vector2u& size) {
  m_windowTitle = title;
  m_windowSize = size;
  m_isFullscreen = false;
  m_isDone = false;
  Destroy();
  Create();
  m_window.setFramerateLimit(60);
}

void Window::Create() {
  auto style = (m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
  m_window.create({m_windowSize.x, m_windowSize.y, 32}, m_windowTitle, style);
}

void Window::Destroy() {
  m_window.close();
}

void Window::BeginDraw() {
  m_window.clear(sf::Color::Black);
}
void Window::EndDraw() {
  m_window.display();
}

bool Window::IsDone() const {
  return m_isDone;
}
bool Window::IsFullscreen() const {
  return m_isFullscreen;
}

void Window::Draw(sf::Drawable& l_drawable) {
  m_window.draw(l_drawable);
}

sf::Vector2u Window::GetWindowSize() {
  return m_windowSize;
}

void Window::ToggleFullscreen() {
  m_isFullscreen = !m_isFullscreen;
  Destroy();
  Create();
}

void Window::Update() {
  sf::Event event;
  while (m_window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      m_isDone = true;
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5) {
      ToggleFullscreen();
    }
  }
}