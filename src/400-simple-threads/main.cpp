#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

// Shared data
std::atomic<int> shared_counter(0);
std::mutex mutex;

// Function to increment the shared counter
void increment_counter(int thread_id, int increments) {
  for (int i = 0; i < increments; ++i) {
    /// Lock the mutex to ensure safe access to shared data
    /// the scope of variables in a for loop ends on EVERY iteration
    /// so the lock is released automatically each time around the loop
    std::lock_guard<std::mutex> lock(mutex);
    ++shared_counter;
    std::cout << "Thread " << thread_id << " incremented counter to " << shared_counter << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  /// Do one locked update with a manual release:
  std::lock_guard<std::mutex> lock(mutex);
  ++shared_counter;
  mutex.unlock();
  /// and one with a scoped automatic unlock
  {
    mutex.lock();  // now break it
    ++shared_counter;
  }

  std::cout << "Thread " << thread_id << " completed " << std::endl;
}

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 *
 * There is a lot in here but it just covers the essentials of drawing stuff to the window.
 */

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(1920, 1080), WINDOW_TITLE, sf::Style::Default, settings};

  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  const int num_threads = 4;  // Number of threads
  const int increments = 5;   // Number of increments per thread

  // Create threads
  std::cout << "starting the threads" << std::endl;
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(increment_counter, i + 1, increments);
  }

  sf::Clock deltaClock{};  /// Keeps track of elapsed time

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      /// This is how you listen for the window close button being pressed
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /// this is how you listen for a specific key being pressed
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
          window.close();
        }
      }
      /// if you want items to stay the same size, you must look after resize events
      /// it can still look scrappy during the resize
      /// This works because the scene is effectively infinite and the window is just
      /// a view of it starting at (0,0)
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    /// always start from a clean slate
    window.clear();

    window.display();
  }
  std::cout << "Start to join the threads" << std::endl;
  // Join threads - ensures all threads complete before continuing
  for (auto& th : threads) {
    th.join();
  }
  std::cout << "All the threads are joined" << std::endl;

  // Display final counter value
  std::cout << "Final counter value: " << shared_counter << std::endl;

  return 0;
}
