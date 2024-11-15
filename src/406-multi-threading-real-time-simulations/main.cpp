#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "button.h"
#include "utils.h"

/***
 * Multiple threads let an application perform several tasks
 * concurrently. For a real-time robot simulation we might have:
 *  - sensor data processing
 *  - control algorithms
 *  - UI rendering
 *  - logging
 *
 * This example uses a game loop with those features - albeit rather
 * simplified.
 *
 */

std::mutex cout_mutex;  /// keep the console IO neat and tidy
std::mutex stateMutex;

/// shared state
struct SystemState {
  float sensorData;     // Simulated sensor data
  float controlOutput;  // Simulated control output
};

// Simulate sensor updates
void sensorUpdate(SystemState& state, std::atomic<bool>& running) {
  while (running) {
    {
      float new_value = static_cast<float>(rand()) / RAND_MAX;  // Random data
      std::lock_guard<std::mutex> lock(stateMutex);
      state.sensorData = exponential_filter(state.sensorData, new_value, 0.9);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 10 Hz update rate
  }
}

/// Simulate control logic
void controlLogic(SystemState& state, std::atomic<bool>& running) {
  while (running) {
    float sensorValue;
    {
      std::lock_guard<std::mutex> lock(stateMutex);
      sensorValue = state.sensorData;
    }

    float controlOutput = sensorValue * 2.0f;  // simple control
    {
      std::lock_guard<std::mutex> lock(stateMutex);
      state.controlOutput = controlOutput;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // 20 Hz update rate
  }
}

/**
 * We might consider a separate UI thread if the work needed was heavy and some
 * concurrent processing was needed. Or, perhaps, if there were some communication
 * with another process or device that would benefit from async operation.
 * BUT SFML requires that drawing happen in the same context as the target creation
 * so everything would have to move to the UI thread.
 * Mostly though, it is simpler to just do the UI presentation stuff in the main
 * program thread.
 */

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "406 multi threading for real time simulation", sf::Style::Default, settings};

  window.setFramerateLimit(60);

  sf::Texture props_texture;
  if (!props_texture.loadFromFile("./assets/images/props-1x1.png")) {
    exit(1);
  }

  sf::Sprite prop(props_texture);
  prop.setTextureRect(sf::IntRect(5 * 160, 6 * 160, 160, 160));
  prop.setScale(0.5, 0.5);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/national-park.otf")) {
    exit(1);
  }

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(16);
  text.setFillColor(sf::Color(140, 91, 54));
  text.setPosition(50, 5);

  SystemState state{0, 0};
  std::atomic<bool> running(true);

  // Create triangles
  const float indicator_size = 8.0f;
  sf::ConvexShape sensorTriangle(3);                                // Triangle for sensor data
  sensorTriangle.setPoint(0, sf::Vector2f(indicator_size, 0.0f));   // Tip
  sensorTriangle.setPoint(1, sf::Vector2f(0.0f, indicator_size));   // Bottom-left
  sensorTriangle.setPoint(2, sf::Vector2f(0.0f, -indicator_size));  // Top-left
  sensorTriangle.setFillColor(sf::Color::Green);

  sf::ConvexShape controlTriangle(3);                                // Triangle for control output
  controlTriangle.setPoint(0, sf::Vector2f(indicator_size, 0.0f));   // Tip
  controlTriangle.setPoint(1, sf::Vector2f(0.0f, indicator_size));   // Bottom-left
  controlTriangle.setPoint(2, sf::Vector2f(0.0f, -indicator_size));  // Top-left
  controlTriangle.setFillColor(sf::Color::Blue);

  sf::RectangleShape gauge_bar;
  gauge_bar.setSize({16, 100});
  gauge_bar.setOutlineThickness(1.0);
  gauge_bar.setOutlineColor(sf::Color::Yellow);
  gauge_bar.setFillColor((sf::Color(255, 255, 255, 64)));

  // Start threads
  std::thread sensorThread(sensorUpdate, std::ref(state), std::ref(running));
  std::thread controlThread(controlLogic, std::ref(state), std::ref(running));

  float x = 0;
  /// now we can do the main loop
  while (window.isOpen()) {
    ////  EVENTS    //////////////////////////////////////////////////////////////////////
    sf::Event event{};
    bool should_close = false;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        should_close = true;
      }
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
          should_close = true;
        }
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    if (should_close) {
      running = false;
      window.close();
    }

    ////  UPDATE    //////////////////////////////////////////////////////////////////////
    // UI calculations in the main thread
    float sensorValue = 0.0f, controlValue = 0.0f;
    {
      std::lock_guard<std::mutex> lock(stateMutex);
      sensorValue = state.sensorData;
      controlValue = state.controlOutput;
    }
    char buf[32];

    sensorTriangle.setPosition(100.0f, 150 - 100 * sensorValue);
    controlTriangle.setPosition(200.0f, 150 - 100 * controlValue / 2.0f);
    text.setString(std::to_string(controlTriangle.getPosition().x));
    ////  DISPLAY   //////////////////////////////////////////////////////////////////////
    window.clear();
    gauge_bar.setPosition(110, 50);
    window.draw(gauge_bar);
    gauge_bar.setPosition(210, 50);
    window.draw(gauge_bar);
    window.draw(sensorTriangle);
    window.draw(controlTriangle);
    text.setString(std::to_string(int(sensorValue * 100.0f)));
    text.setPosition(108, 150);
    window.draw(text);
    sprintf(buf, "%3d", int(controlValue * 100.0f));
    text.setString(buf);
    text.setPosition(203, 150);
    window.draw(text);
    window.display();
    //////////////////////////////////////////////////////////////////////////////////////
  }

  running = false;

  std::cout << "Start to join the threads" << std::endl;
  // Join threads
  if (sensorThread.joinable()) {
    sensorThread.join();
  }
  if (controlThread.joinable()) {
    controlThread.join();
  }
  /// wait for the simple threads to terminate before we continue
  std::cout << "All the threads are joined" << std::endl;

  return 0;
}
