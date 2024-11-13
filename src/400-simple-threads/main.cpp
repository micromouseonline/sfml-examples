#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

/**
 * Threads are a way to allocate many tasks in a process to their own execution unit.
 * they execute code concurrently rather than in time slices as might be the case for
 * running multiple tasks in a RTOS. This is only true where the hardware permits. Any
 * modern processor in a PC will run threads concurrently. Simpler processors will
 * only run a single thread or task at a time. Even on a modern desktop processor,
 * individual threads may run in time slices depending on the availability of cores.
 *
 * Since C++11 you can do these things:
 *
 * It is important to distinguish between a thread object (std::thread) and the actual
 * thread which is a piece of code - normally a function. Unfortunately C++ uses
 * the same word for both things
 *
 * Create an empty thread object. This is not actually a thread, it just provides a way
 * to manage a thread.No code is associated with it. Nothing gets executed.
 * The thread object is in a non-joinable state. Attempts to call join() or detach()
 * will generate an exception.
 *
 *     std::thread worker;
 *
 * Assign code - an actual thread - to a thread object by creating a thread with the
 * name of a function:
 *
 *     worker = std::thread(activity);
 *
 * The function used as the argument does not necessarily have to be void and can take
 * arguments. This is fine for basic cases but look into std::bind and/or lambdas for
 * more complex requirements:
 *     void activity(int count);
 *     worker = std::thread(activity, 5);
 *
 * The thread will now be running and will continue to do so until the thread function
 * exits. If it does not exit, the main thread may terminate but these other threads will
 * persist and may cause problems as there is nothing left running to control them.
 *
 * It is crucial that the thread lifecycle is properly managed. Always ensure that the
 * thread function will return. Forcibly terminating a thread is likely to cause
 * problems like memory leaks or corrupted resources.
 *
 * If there are circumstances where a thread might not have an internal reason to terminate
 * - as might be the case in a simulation, consider a global flag that can be set by the main
 * thread, telling the worker it is time to quit.
 *
 * See the endless_task example below:
 *
 * After the thread function terminates, it is in an 'exited' state. No code is running. Unless
 * it was detached (see below), the thread is now joinable and your main thread can call the
 * thread object's join() method to ensure its resources (memory, stack and so on) are properly
 * released. Calling join() on a thread that has not terminated will cause the main thread to
 * wait until that thread does terminate.
 *
 * If a thread has terminated AND it has been joined, the thread object is no longer joinable
 * and cannot be reused. To assign a new worker, you must create a new std::thread object.
 *
 * If a thread has started but NOT been joined, it is still joinable - even if it has
 * terminated and you must call join() before the std::thread object can be destructed
 * or reused.
 *
 *
 *
 *
 *
 *
 * std::atomic<bool> shouldRun(true);
 *
 * void worker_task(){
 * while (shouldRun){
 *  // do work
 * }
 *
 *
 * std::thread worker(worker_task);
 *
 *
 *
 */

/**
 * Function to safely increment a shared counter.
 *
 * This will be the code that is run by each thread.
 *
 * @param thread_id
 * @param increments
 */

// Shared data
std::atomic<int> shared_counter(0);
std::mutex counter_mutex;

std::atomic<int> worker_boxes(0);
std::mutex worker_mutex;

void simple_task(int thread_id, int increments) {
  for (int i = 0; i < increments; ++i) {
    /// the counter is declared atomic and does not need a mutex
    ++shared_counter;
    /// But the output stream is much more complex and must be
    /// protected with a mutex. Although the scope of variables in a for loop
    /// ends on EVERY iteration so the lock would be released automatically
    /// each time around the loop, we release it immediately so that the
    /// output stream is locked for the shortest possible time. Like the
    /// for loop body, a block scope mutex is automatically released when
    /// it goes out of scope
    {
      std::lock_guard<std::mutex> lock(counter_mutex);
      std::cout << "Thread " << thread_id << " incremented counter to " << shared_counter << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(666));
  }
}

/***
 * The endless task does not expect to return until it is told that it should.
 * This is done by means pf a shared flag that the task can test. In the main()
 * function the code simply sets must_carry_on to be false to terminate the task.
 */
std::atomic<bool> keep_making_boxes(true);
void endless_task() {
  while (keep_making_boxes) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    worker_boxes++;
  }
  std::lock_guard<std::mutex> lock(worker_mutex);
  std::cout << "Worker finished after " << worker_boxes << " boxes." << std::endl;
}

int main() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(800, 600), "Simple Threads Demo", sf::Style::Default, settings};

  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  sf::Texture texture;
  if (!texture.loadFromFile("./assets/images/wooden-chest-45x40.png")) {
    exit(1);
  }
  sf::Sprite chest(texture);

  sf::Font dm_font;
  if (!dm_font.loadFromFile("./assets/fonts/GortonDigital.otf")) {
    exit(1);
  }

  sf::Text text;
  text.setFont(dm_font);
  text.setCharacterSize(32);
  text.setFillColor(sf::Color(140, 91, 54));
  text.setPosition(50, 5);

  /**
   * Starting the simple threads
   * Each can only increment by one at a time
   * They do not run sequentially
   **/
  std::cout << "Start the simple threads " << std::endl;
  const int num_threads = 4;  /// Number of threads
  const int increments = 5;   /// Number of increments per thread
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(simple_task, i, increments);
  }

  /// this immediately kicks off the task
  std::thread worker(endless_task);

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
    }
    ////  UPDATE    //////////////////////////////////////////////////////////////////////

    ////  DISPLAY   //////////////////////////////////////////////////////////////////////
    window.clear();

    char buf[32];
    sprintf(buf, "BOXES: %3d", (int)worker_boxes);
    text.setString(buf);
    text.setPosition(50, 5);
    window.draw(text);
    sprintf(buf, "Shared Counter: %3d", (int)shared_counter);
    text.setString(buf);
    text.setPosition(350, 5);
    window.draw(text);

    for (int i = 0; i < worker_boxes; i++) {
      float x = 50.0f * (1.0f + i % 10);
      float y = 50.0f * (1.0f + i / 10);
      chest.setPosition(x, y);
      window.draw(chest);
    }

    window.display();
    //////////////////////////////////////////////////////////////////////////////////////
  }

  /// wait for the simple threads to terminate before we continue
  std::cout << "Start to join the threads" << std::endl;
  for (auto& th : threads) {
    th.join();
  }

  /// now terminate the worker thread
  keep_making_boxes = false;
  worker.join();

  std::cout << "All the threads are joined" << std::endl;

  /// Display final counter value
  std::cout << "Final counter value: " << shared_counter << std::endl;

  return 0;
}
