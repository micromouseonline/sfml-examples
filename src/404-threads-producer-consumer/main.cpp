#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
#include "button.h"

/**
 * Producer-consumer systems are a classic application type where
 * one process, or thread, produces something that is consumer by
 * another process or thread.
 *
 * Suppose we have a thread that generates log messages - a producer
 * Another thread might periodically collect (consume) those messages
 * for some purpose.
 *
 * The producer and consumer are asynchronous
 *
 * One way to manage this relationship is by means of a queue.
 *   - The Producer thread generates log messages and pushes them to a
 *     shared queue.
 *   - The Consumer thread retrieves messages from the queue and
 *     processes them.
 *
 * The queue is protected by a std::mutex, and a std::condition_variable
 * is used to signal when new data is available for the consumer.
 *
 */
std::queue<std::string> log_queue;   // Shared queue for log messages
std::mutex queue_mutex;              // Mutex to protect the queue
std::atomic<bool> finished = false;  // Signal for producer shutdown
std::mutex cout_mutex;               // Mutex to protect the iostream

std::condition_variable production_control;
std::atomic<bool> producing = false;  // Signal for producer shutdown

// Producer thread function
void producer() {
  int i = 0;
  while (true) {
    {
      std::unique_lock lock(queue_mutex);
      production_control.wait(lock, [] { return producing || finished; });
      if (finished) {
        break;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 500));  // Simulate work
    std::string message = "Barrel " + std::to_string(i++);
    // Lock the queue and push a new log message
    {
      std::lock_guard lock(queue_mutex);
      log_queue.push(message);
    }
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    std::cout << "Produced: " << message << std::endl;
  }

  // Signal that production is finished so the consumer knows to exit
  // when the queue is empty
  finished = true;
}

// Consumer thread function processes all the items in the queue
void consumer() {
  while (true) {
    for (int i = 0; i < 10; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Simulate processing time
      if (finished) {
        break;
      }
    }
    std::lock_guard lock(queue_mutex);
    // Process all available messages
    while (!log_queue.empty()) {
      std::string message = log_queue.front();
      log_queue.pop();
      std::lock_guard cout_lock(cout_mutex);
      std::cout << "          Consumed: " << message << std::endl;
    }
    if (finished && log_queue.empty()) {
      break;
    }
  }
}

size_t get_queue_size() {
  std::lock_guard lock(queue_mutex);
  return log_queue.size();
}

int main() {
  std::srand(static_cast<unsigned int>(std::time(0)));  // Initialize random seed
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "Producer-Consumer Demo", sf::Style::Default, settings};

  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  sf::Font font;
  if (!font.loadFromFile("./assets/fonts/national-park.otf")) {
    exit(1);
  }

  sf::Texture props;
  if (!props.loadFromFile("./assets/images/props-1x1.png")) {
    exit(1);
  }

  sf::Sprite chest(props);
  chest.setTextureRect(sf::IntRect(8 * 160, 6 * 160, 160, 160));
  chest.setScale(1.0, 1.0);

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(24);
  text.setFillColor(sf::Color(255, 255, 0, 64));

  float w = 80;
  float h = 24;
  Button start_button(400, 300, w, h, "START", font);

  std::thread producer_thread(producer);
  std::thread consumer_thread(consumer);

  /// now we can do the main loop
  while (window.isOpen()) {
    ////  EVENTS    //////////////////////////////////////////////////////////////////////
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
          window.close();
        }
      }
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
      ButtonEvent buttonEvent = start_button.handleEvent(event, window);
      if (buttonEvent == ButtonEvent::Released) {
        producing = !producing;
        production_control.notify_all();
        if (producing) {
          start_button.set_text("STOP");
        } else {
          start_button.set_text("START");
        }
      }
    }
    ////  UPDATE    //////////////////////////////////////////////////////////////////////

    ////  DISPLAY   //////////////////////////////////////////////////////////////////////
    window.clear();
    // draw a picture to show the queue size
    text.setString("Queued Items:");
    text.setPosition(64.0f, 78.0f);
    window.draw(text);
    for (size_t i = 0; i < get_queue_size(); i++) {
      chest.setPosition(static_cast<float>(i) * 40.0f, 50.0f);
      window.draw(chest);
      text.setString(std::to_string(i));
      text.setPosition(static_cast<float>(i) * 40.0f + 72, 135.0f);
      window.draw(text);
    }

    start_button.draw(window);

    window.display();
    //////////////////////////////////////////////////////////////////////////////////////
    ///
  }
  finished = true;
  producing = false;
  production_control.notify_all();

  /// wait for the threads to terminate before we continue
  producer_thread.join();
  consumer_thread.join();
  // no need to lock - all the threads are done;
  std::cout << "All the threads are joined" << std::endl;

  return 0;
}
