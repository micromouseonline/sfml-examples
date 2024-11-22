//
// Created by peter on 22/11/24.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "drawing.h"
#include "event_observer.h"
#include "maze.h"
#include "robot-control.h"
#include "robot-display.h"
#include "robot.h"
#include "sensor-data.h"
#include "textbox.h"
#include "window.h"

class Application : public IEventObserver {
 public:
  Application() : m_window("Application", sf::Vector2u(800, 600)) {
    RestartClock();
    m_elapsed = sf::Time::Zero;
    srand(time(nullptr));  // just the simple random number seed;
    m_textbox.Setup(5, 12, 300, sf::Vector2f(450, 10));
    m_textbox.Add("Hello World!");
    m_window.AddObserver(this);
  };

  ~Application() = default;

  void Run() {
    while (!GetWindow()->IsDone()) {
      HandleInput();
      Update();
      Render();
      RestartClock();
    }
  };

  void OnEvent(const Event& event) override {
    switch (event.type) {
      case EventType::SFML_EVENT:
        if (event.event.type == sf::Event::MouseWheelScrolled) {
          std::string msg = "SFML Event - Mouse Wheel Scrolled ";
          event.event.mouseWheelScroll.delta > 0 ? msg += "Up" : msg += "Down";
          m_textbox.Add(msg);
        }
        break;
      case EventType::USER_EVENT:
        m_textbox.Add("USER Event");
        break;
      default:
        m_textbox.Add("UNHANDLED Event");
        break;
    }
  }
  /***
   * Note that this is not about the events. We can test the state of the mouse and
   * the keyboard or any other input devices.
   * We are not looking for things that have happened like keypress events.
   * Could this get tricky if we want to do something like enter text? We will see.
   */
  void HandleInput() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      mp = sf::Vector2f(sf::Mouse::getPosition(*m_window.GetRenderWindow()));
      std::string msg = "Left Mouse button pressed";
      m_textbox.Add(msg);
    }
  };

  /// This is where the work gets done. All the application logic and behaviour
  /// is performed from this method. By passing in a deltaTime we can choose to
  /// perform updates once per frame or at a higher, or lower, fixed time interval.
  /// No rendering is done from this method.
  void Update(sf::Time deltaTime = sf::seconds(0.01)) {
    m_window.Update();  // call this first to process window events
    m_elapsed += deltaTime;
  };

  /// The Render() method is the only place that output is generated for the
  /// window - and any audio devices if used. It is called after the Update()
  /// method and will may be called once per frame or after every update depending
  /// on the underlying update strategy.
  void Render() {
    m_window.BeginDraw();
    sf::RenderWindow& window = *m_window.GetRenderWindow();
    draw_line(window, {40, 90}, mp, sf::Color::Red);
    m_textbox.Render(window);
    m_window.EndDraw();
  }

  Window* GetWindow() {
    return &m_window;  //
  }

  sf::Time GetElapsed() {
    return m_elapsed;  //
  }

  void RestartClock() {
    m_clock.restart();  //
  }

  /// This would be a good place to create any overlay information or to log
  /// performance data for example.
  void UpdateStatistics(sf::Time elapsedTime) {
    //
  };

 private:
  sf::Vector2f mp = {0, 0};
  static const sf::Time TimePerFrame;
  Window m_window;
  sf::Clock m_clock;
  sf::Time m_elapsed;

  sf::Font mFont;
  sf::Text mStatisticsText;
  sf::Time mStatisticsUpdateTime;
  Textbox m_textbox;
  std::size_t mStatisticsNumFrames;
};

#endif  // APPLICATION_H
