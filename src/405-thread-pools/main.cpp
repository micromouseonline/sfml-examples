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
/***
 * It is sometimes useful to be able to maintain a pool of ready to use,
 * pre-initialised threads to execute tasks. Tasks can be assigned to
 * threads as needed.
 *
 * We need:
 *
 *  - the worker threads, pre-created and ready to go
 *  - a task queue that is thread-safe to store tasks waiting to execute
 *  - a means of synchronisation - mutexes and condition variables.
 *
 */

/// A class implements the thread pool
class ThreadPool {
 public:
  explicit ThreadPool(size_t threadCount) : stop(false) {
    for (size_t i = 0; i < threadCount; ++i) {
      workers.emplace_back([this] {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
              return;
            task = std::move(tasks.front());
            tasks.pop();
          }
          task();  // actually do the thing
        }
      });
    }
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
      if (worker.joinable())
        worker.join();
    }
  }

  void enqueueTask(std::function<void()> task) {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      if (stop)
        throw std::runtime_error("ThreadPool is stopped");
      tasks.emplace(std::move(task));
    }

    condition.notify_one();
  }

 private:
  std::vector<std::thread> workers;         // Worker threads
  std::queue<std::function<void()>> tasks;  // Task queue
  std::mutex queueMutex;                    // Mutex for task queue
  std::condition_variable condition;        // Condition variable for synchronization
  std::atomic<bool> stop;                   // Stop flag
};

// Example function that will serve as a task
std::mutex cout_mutex;
void exampleTask(int taskId) {
  {
    std::lock_guard lock(cout_mutex);
    std::cout << "Task " << taskId << " is running on thread " << std::this_thread::get_id() << "\n";
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "Thread pool", sf::Style::Default, settings};

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
  text.setCharacterSize(32);
  text.setFillColor(sf::Color(140, 91, 54));
  text.setPosition(50, 5);

  float w = 80;
  Button start_button((800 - w) / 2, 300, w, 30, "START", font);
  bool started = false;

  std::atomic<bool> stop_all_threads = false;

  ThreadPool pool(4);

  for (int i = 1; i <= 18; ++i) {
    pool.enqueueTask([i] { exampleTask(i); });
  }

  /// now we can do the main loop
  while (window.isOpen()) {
    ////  EVENTS    //////////////////////////////////////////////////////////////////////
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        stop_all_threads = true;
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
      if (!started && buttonEvent == ButtonEvent::Released) {
        started = true;
      }
    }
    ////  UPDATE    //////////////////////////////////////////////////////////////////////

    ////  DISPLAY   //////////////////////////////////////////////////////////////////////
    if (!started) {
      window.clear();
      start_button.draw(window);
      window.display();
      continue;
    }

    window.clear();

    window.draw(text);
    window.display();
    //////////////////////////////////////////////////////////////////////////////////////
  }

  /// wait for the simple threads to terminate before we continue
  std::cout << "Start to join the threads" << std::endl;

  std::cout << "All the threads are joined" << std::endl;

  return 0;
}
