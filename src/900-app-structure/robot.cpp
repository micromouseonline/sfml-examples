//
// Created by peter on 23/11/24.
//

#include "robot.h"
#include <iostream>
#include <thread>

Robot::Robot() : m_running(false), m_pose(0.0f, 0.0f), m_orientation(0.0f) {
}

Robot::~Robot() {
  Stop();
}

void Robot::Start() {
  if (!m_running) {
    m_running = true;
    m_thread = std::thread(&Robot::Run, this);
  }
}

void Robot::Stop() {
  if (m_running) {
    m_running = false;
    if (m_thread.joinable()) {
      m_thread.join();
    }
  }
}

void Robot::Run() {
  /// for testing, just have the robot run around in a circle.
  const sf::Vector2f center(300.0f, 300.0f);                       // Center of the circle
  const float radius = 200.0f;                                     // Radius of the circle
  const float angularSpeed = 60.0f;                                // Degrees per second
  const float angularSpeedRad = angularSpeed * 3.14159f / 180.0f;  // Radians per second

  sf::Clock clock;
  while (m_running) {
    sf::Time elapsed = clock.restart();  // Time since last update
    float deltaTime = elapsed.asSeconds();

    // Calculate the current angle based on elapsed time
    static float angle = 0.0f;
    angle += angularSpeedRad * deltaTime;

    // Keep angle within [0, 2π]
    if (angle >= 2 * 3.14159f) {
      angle -= 2 * 3.14159f;
    }

    // Update robot's position and orientation
    {
      std::lock_guard<std::mutex> lock(m_stateMutex);
      m_pose.x = center.x + radius * std::cos(angle);  // x = cx + r * cos(θ)
      m_pose.y = center.y + radius * std::sin(angle);  // y = cy + r * sin(θ)
      m_orientation = 2 * angle;                       // Orientation matches the tangent angle
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 100 Hz loop
  }
}

void Robot::UpdateState() {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  // Update the robot's position and orientation (simulation logic here)
}

void Robot::ProcessControl() {
  // Call RobotControl to compute control actions based on the sensor data
  m_control.Update(m_sensorData);
}

sf::Vector2f Robot::GetPose() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_pose;
}

float Robot::GetOrientation() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_orientation;
}

void Robot::UpdateSensors(const SensorData& sensorData) {
  // Update sensor data (thread-safe mechanism if needed)
  //  m_sensorData = sensorData;
}
