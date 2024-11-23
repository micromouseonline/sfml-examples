//
// Created by peter on 22/11/24.
//

#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <thread>
#include "collision-object.h"
#include "robot-control.h"
#include "robot-model.h"
#include "sensor-data.h"
#include "sensor.h"

/// forward declarations
class Application;

/***
 * This is the robot class.
 */
class Robot {
 public:
  Robot();
  ~Robot();

  // Starts the robot in its own thread
  void Start(Application *app);

  // Stops the robot's thread
  void Stop();

  // Accessors for robot state (thread-safe)
  sf::Vector2f GetPose() const;  // Returns (x, y) position
  float GetOrientation() const;  // Returns orientation in radians

  // This must request sensor data from the Application
  void ReadSensorValues();

  /// A getter for the Robot's sensor values
  SensorValues &GetSensorValues();

  void StartSystick();

  void StopSystick();

  void systick();

  uint32_t millis();

 private:
  // Main loop executed in the robot thread
  void Run();

  // Low-level control methods
  void UpdateState();
  void ProcessControl();
  float lfs = 0.0f;
  float lds = 0.0f;
  float rds = 0.0f;
  float rfs = 0.0f;
  uint32_t ticks = 0;
  std::thread m_systick_thread;
  std::thread m_thread;
  std::atomic<bool> m_running;  // Thread control flag
  sf::Vector2f m_pose;          // (x, y) position
  float m_orientation;          // Orientation in radians

  mutable std::mutex m_stateMutex;       // Protects access to m_pose and m_orientation
  SensorValues m_sensorValues;           // Current sensor readings
  RobotControl m_control;                // Higher-level control logic
  Application *m_application = nullptr;  // Pointer to the Application (for querying sensor readings)
};

#endif  // ROBOT_H
