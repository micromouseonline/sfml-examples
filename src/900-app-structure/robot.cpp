//
// Created by peter on 23/11/24.
//

#include "robot.h"
#include <iostream>
#include <thread>
#include "application.h"

Robot::Robot() : m_running(false), m_pose(300.0f, 300.0f), m_orientation(0.0f) {
}

Robot::~Robot() {
  Stop();
}

void Robot::Start(Application* app) {
  if (!m_running) {
    m_application = app;
    m_running = true;
    m_thread = std::thread(&Robot::Run, this);
    m_systick_thread = std::thread([this]() {
      while (m_running) {
        systick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    });
  }
}

void Robot::Stop() {
  if (m_running) {
    m_running = false;
    if (m_systick_thread.joinable()) {
      m_systick_thread.join();
    }
    if (m_thread.joinable()) {
      m_thread.join();
    }
  }
}

void Robot::systick() {
  ticks++;
  ReadSensorValues();
}

void Robot::Run() {
  /// TODO: for testing, just have the robot run around in a circle.
  const sf::Vector2f initialCenter(300.0f, 300.0f);  // Center of the circle
  const float initialRadius = 100.0f;                // Radius of the circle
  const float initialAngularSpeed = 60.0f;           // Degrees per second

  m_control.SetCircularTrajectory(initialCenter, initialRadius, initialAngularSpeed);

  sf::Clock clock;
  while (m_running) {
    sf::Time elapsed = clock.restart();  // Time since last update
    float deltaTime = elapsed.asSeconds();
    deltaTime = 0.01f;

    // Get the velocities from the RobotControl class
    float linearVelocity = m_control.GetLinearVelocity();
    float angularVelocity = m_control.GetAngularVelocity();

    {
      std::lock_guard<std::mutex> lock(m_stateMutex);

      // Update orientation
      m_orientation += angularVelocity * deltaTime;

      // Keep orientation within [0, 360) degrees
      if (m_orientation >= 360.0f) {
        m_orientation -= 360.0f;
      } else if (m_orientation < 0.0f) {
        m_orientation += 360.0f;
      }

      // Update position based on linear velocity and orientation
      float orientationRad = m_orientation * 3.14159f / 180.0f;  // Convert to radians for trigonometric functions
      m_pose.x += linearVelocity * std::cos(orientationRad) * deltaTime;
      m_pose.y += linearVelocity * std::sin(orientationRad) * deltaTime;
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
  m_control.Update(m_sensorValues);
}

sf::Vector2f Robot::GetPose() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_pose;
}

float Robot::GetOrientation() const {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_orientation;
}

void Robot::ReadSensorValues() {
  if (m_application) {
    m_sensorValues = m_application->GetSensorValues();
  }
}

SensorValues& Robot::GetSensorValues() {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return m_sensorValues;
}

uint32_t Robot::millis() {
  std::lock_guard<std::mutex> lock(m_stateMutex);
  return ticks;
  ;
}
