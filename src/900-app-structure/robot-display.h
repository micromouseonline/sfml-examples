//
// Created by peter on 02/11/2024.
//

#ifndef ROBOTVIEW_H
#define ROBOTVIEW_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <mutex>
#include <optional>
#include "event_observer.h"
#include "robot-control.h"
#include "robot.h"
#include "thread-safe-queue.h"

/***
 * The RobotDisplay class renders the robot to the screen and handles any
 * user interaction - for instance, starting, stopping, stepping,
 * mode changes...
 *
 * Instances of this class will run in the main application thread and so it
 * must be made thread safe.
 *
 * To communicate with the RobotControl class, shared variables must be used
 * and guarded with a mutex. Condition variables can also be used to signal
 * changes in state.
 *
 * Thread safe queues can also be used to pass messages between threads. This
 * is particularly useful for simulating logging and telemetry.
 *
 * Core Features of RobotDisplay
 * Rendering:
 *    - Visualize the robot's position and orientation in the simulation environment.
 *    - Render additional elements, such as the maze or sensor data, for better debugging and feedback.
 * User Interaction:
 *    - Handle events such as starting, stopping, stepping the simulation, or changing modes.
 * Thread Safety:
 *    - Use proper synchronization (std::mutex and std::condition_variable) when interacting with
 *    RobotControl to avoid race conditions.
 * Simulation Control:
 *    - Provide an interface for controlling the simulation (e.g., play/pause, reset, or single-step mode).
 *
 * The RobotDisplay runs in the main application thread.
 *
 */

class RobotDisplay : IEventObserver {
 public:
  RobotDisplay(RobotControl& robotControl, sf::RenderWindow& window, ThreadSafeQueue& logQueue)
      : mRobotControl(robotControl), mWindow(window), mLogQueue(logQueue) {}

  void Update(float deltaTime) {
    (void)deltaTime;
    // what do we need to do here?
    // update the sprite
    // do the collision detection with the maze?
  }

  void Render() {
    // Draw maze grid and walls (example)
    // Assume mRobotControl.mMaze has a draw() method
    //    mRobotControl.mMaze.draw(mWindow);

    // Draw the robot
    {
      sf::Vector2f robotPosition = mRobotControl.getRobotPosition();
      sf::CircleShape robotShape(10);  // Represent the robot as a circle
      robotShape.setPosition(robotPosition);
      robotShape.setFillColor(sf::Color::Blue);
      robotShape.setOrigin(10, 10);                                                     // Center origin for rotation
      robotShape.setRotation(mRobotControl.getRobotOrientation() * 180.0f / 3.14159f);  // Convert radians to degrees
      mWindow.draw(robotShape);
      mRobotControl.updateSensors();
    }
  }

 private:
  RobotControl& mRobotControl;
  sf::RenderWindow& mWindow;
  Maze mMaze;
  ThreadSafeQueue& mLogQueue;

  /// this needs to use the IEventHandler class because
  /// events are collected and dispatched by the Window class
  void OnEvent(const Event& event) override {
    switch (event.type) {
      case EventType::SFML_EVENT: {
        sf::Event ev = event.event;
        if (ev.type == sf::Event::KeyPressed) {
          if (ev.key.code == sf::Keyboard::P) {  // Pause
            mRobotControl.requestStop();
          }
          if (ev.key.code == sf::Keyboard::S) {  // Single step
                                                 // Logic for stepping simulation
          }
          if (ev.key.code == sf::Keyboard::R) {  // Reset
                                                 // Logic for resetting simulation
          }
        }
        break;
      }
      case EventType::USER_EVENT:

        break;
      default:

        break;
    }
  }

  void displayLogMessages() {
    std::string message;
    while (mLogQueue.tryPop(message)) {  // Fetch message into the string
      std::cout << message << std::endl;

      // Optionally, render messages on the window
    }
  }
};

////////////////////////////////////////////////////////////////////////////////////////
#if 0

Stuff from the current visual representation

class RobotDisplayView {
 public:
  RobotDisplayView(Robot& robot) : m_robot(robot) {
    m_shape.setSize(sf::Vector2f(robot.m_width, robot.m_height));
    m_shape.setOrigin(robot.m_width / 2, robot.m_height / 2);
    m_shape.setFillColor(sf::Color::Green);
    m_shape.setPosition(m_robot.m_x, m_robot.m_y);
    sf::Image image;
    image.create(robot.m_width, robot.m_height, sf::Color::Blue);
    m_texture.loadFromImage(image);
    m_sprite.setTexture(m_texture);
    m_sprite.setOrigin(robot.m_origin_x, robot.m_origin_y);
    m_sprite.setPosition(m_robot.m_x, m_robot.m_y);
  }

  bool load_texture(const std::string& filename) {
    if (!m_texture.loadFromFile(filename)) {
      std::cerr << "Unable to load texture\n";
      return false;
    }
    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect(0, 0, m_robot.m_width, m_robot.m_height));
    m_sprite.setOrigin(m_robot.m_origin_x, m_robot.m_origin_y);
    update();
    return true;
  }

  void update() {
    m_shape.rotate(m_robot.m_angle);
    m_shape.setPosition(m_robot.m_x, m_robot.m_y);
    m_sprite.setRotation(m_robot.m_angle);
    m_sprite.setPosition(m_robot.m_x, m_robot.m_y);
    // setTextureRect(sf::IntRect(m_robot.m_state * m_robot.m_width, 0, m_robot.m_width, m_robot.m_height));
    //    setRotation(m_robot.m_angle);
    //    setPosition(m_robot.m_x, m_robot.m_y);
  }
  void draw(sf::RenderTarget& target) {
    if (m_sprite.getTexture()) {
      target.draw(m_sprite);
    } else {
      target.draw(m_shape);
    }
  }

 private:
  Robot& m_robot;
  sf::RectangleShape m_shape;
  sf::Sprite m_sprite;
  sf::Texture m_texture;
};
#endif
#endif  // ROBOTVIEW_H
