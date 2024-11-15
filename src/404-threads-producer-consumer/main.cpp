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
 *
 * Using multiple threads inevitably increases the apparent complexity
 * of an application when you need to keep everything safe and
 * reliable. That is the tradeoff for code that can run more effectively
 * with many concurrent processes.
 *
 * This example illustrates how processes can synchronise operations
 * using a queue. A producer generates items - simple string messages
 * in this case - at a fairly random rate. Each item is added to a
 * shared queue. Another process consumes these items at a fixed rate.
 * Here, the consumer will process all the items in the queue every
 * two seconds. In this way, the consumer only needs to wake up
 * every two seconds to handle the queued item and so uses very
 * few resources. To make sure that the consumer is responsive, the
 * two second sleep is split into 200ms slices while testing a
 * shared application flag to see if the entire application must
 * shut down. Without that there would be a delay at the end of the
 * application of up to two seconds while the consumer thread is sleeping.
 *
 *
 *
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
 * Queues are often a good choice for a shared data structure in threaded
 * applications. Most of the time the operations at the head and tail of
 * the queue do not overlap. Even so care must be taken to prevent race
 * conditions where two or more thread attempt to access the same memory
 * areas at the same time.
 *
 * Block scope mutex locks are used whenever a thread reads or modifies
 * shared memory.
 *
 * This demonstration also uses condition variables. These are used to
 * block a thread until another thread modifies a shared variable.
 * When the wait() method is called, it is given a reference to a
 * std::unique_lock object that it can unlock while waiting and re-lock
 * when finished.
 *
 * The modifying thread can then lock the resources while it is modified.
 * After any modification, this thread uses the same condition variable
 * as the waiting threads and calls its notify_one() or notify_all()
 * methods. These calls will unblock any threads that are waiting for
 * this condition variable. Use notify_one() if only one thread is
 * waiting. Use notify_all() if there are several waiting threads.
 *
 * In the example below, the production_control variable is used to
 * start and stop production by the producer. There is a shared variable
 * called producing that turns on and off the production but if the
 * producer used a simple test for that flag, and entered a busy-wait
 * loop until it were true, the thread would be wasting processor power.
 *
 * Instead, the condition variable suspends the thread until it is
 * notified that the flag(s) have been changed.
 *
 * Note that there are other ways to do this. The example serves
 * to illustrate a particular circumstance.
 *
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
      /// use a condition variable to decide if we should start production
      /// or quit altogether. The thread will block here if not producing
      /// setting the finished flag will also unblock the thread
      std::unique_lock lock(queue_mutex);
      production_control.wait(lock, [] { return producing || finished; });
      if (finished) {
        break;
      }
    }

    /// simulate work at semi-random intervals
    std::this_thread::sleep_for(std::chrono::milliseconds(100 + rand() % 500));
    std::string message = "Barrel " + std::to_string(i++);
    {
      /// add the message to the queue safely
      std::lock_guard lock(queue_mutex);
      log_queue.push(message);
    }
    /// std::cout is a shared resource and must be guarded
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    std::cout << "Produced: " << message << std::endl;
  }
}

// Consumer thread function processes all the items in the queue
void consumer() {
  while (true) {
    for (int i = 0; i < 10; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Simulate processing time
      if (finished) {
        break;  // terminate the wait loop only
      }
    }
    /// Process all available messages while guarding the queue
    std::lock_guard lock(queue_mutex);
    while (!log_queue.empty()) {
      std::string message = log_queue.front();
      log_queue.pop();
      std::lock_guard cout_lock(cout_mutex);
      std::cout << "          Consumed: " << message << std::endl;
    }
    /// ensure the entire queue is always processed
    if (finished && log_queue.empty()) {
      break;
    }
    /// lock guard is released here as it goes out of scope
  }
}

/**
 * provide a thread-safe way to get the length of the queue
 */
size_t get_queue_size() {
  std::lock_guard lock(queue_mutex);
  return log_queue.size();
}

int main() {
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
        /// make sure the producer is notified if it is waiting
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
    size_t length = get_queue_size();
    text.setString("Queued Items: " + std::to_string(length));
    text.setPosition(64.0f, 78.0f);
    window.draw(text);
    for (size_t i = 0; i < length; i++) {
      chest.setPosition(static_cast<float>(i) * 40.0f, 50.0f);
      window.draw(chest);
      text.setString(std::to_string(i));
      text.setPosition(static_cast<float>(i) * 40.0f + 72, 135.0f);
      window.draw(text);
    }

    start_button.draw(window);
    window.display();
    //////////////////////////////////////////////////////////////////////////////////////
  }
  /// signal the threads that they are done.
  finished = true;
  producing = false;
  /// make sure they get the message
  production_control.notify_all();

  /// wait for the threads to terminate before we continue
  producer_thread.join();
  consumer_thread.join();
  /// no need to lock std::cout here - all the threads are done;
  std::cout << "All the threads are joined" << std::endl;

  return 0;
}
