//
// Created by peter on 22/11/24.
//

#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

/**
 *
 * The RobotControl class is responsible for the behavior of the robot in the
 * environment made available by the RobotDisplay class.
 *
 * For a micromouse, this includes path planning, searching, mapping,
 * and other decision-making behaviors. It can be adapted for other types of robots.
 *
 * The RobotControl class holds an instance of the Robot class and a logical
 * representation of the Maze. The physical maze is maintained and updated by
 * the main thread.
 *
 * Synchronization between RobotControl and RobotDisplay is managed using
 * condition variables. RobotControl sets a request flag and calls notify_all().
 * RobotDisplay responds by calculating sensor readings appropriate to the current
 * robot state and updating the Robot instance.
 *
 * Core Features of RobotControl
 * Behavioral Logic:
 *    - Implements decision-making algorithms such as path planning, mapping, and navigation.
 *    - Reacts to sensor data (e.g., front distance, wall proximity) for obstacle avoidance or exploration.
 *
 * Synchronization:
 *    - Uses a std::condition_variable to coordinate updates between threads:
 *    - Notifies the RobotDisplay thread to update sensor data.
 *    - Waits for the updated data to proceed with control logic.
 *
 * Robot Interaction:
 *    - Directly interacts with the Robot instance to access pose, velocity, or other state data.
 *
 * Environment Access:
 *    - Maintains a reference to a Maze object, which represents the logical environment
 *      for pathfinding and navigation.
 *
 * mRobot, mMaze and mSensorData are private members of RobotControl accessed
 * through methods that ensure thread safety
 *
 */

#include <condition_variable>
#include <mutex>
#include "maze.h"
#include "robot.h"
#include "sensor-data.h"
#include "thread-safe-queue.h"

class RobotControl {
 public:
  RobotControl(Robot& robot, Maze& maze, SensorData& sensorData, ThreadSafeQueue& logQueue)
      : mRobot(robot), mMaze(maze), mSensorData(sensorData), mRunning(false), mLogQueue(logQueue) {
    //
  }

  void logMessage(const std::string& message) {
    mLogQueue.push(message);  //
  }

  void run() {
    std::unique_lock<std::mutex> lock(mMutex);
    mRunning = true;
    logMessage("Starting my run!");

    while (mRunning) {
      updateSensors();           // Notify for updated sensor readings
      mConditionVar.wait(lock);  // Wait for sensor update notification
      logMessage("Sensor data updated!");

      // Process sensor data
      if (mSensorData.getFrontDistance() < 100) {
        logMessage("Obstacle detected ahead!");
        // Example: Stop robot if obstacle is too close
        // mRobot.setInputVelocities(0.0, 0.0);
      } else {
        logMessage("Path ahead clear!");
        // Example: Continue forward movement
        // mRobot.setInputVelocities(0.2, 0.0);  // 0.2 m/s forward, no rotation
      }

      // Example: Update logical maze
      // mMaze.updateFromSensors(mSensorData);
    }
  }

  sf::Vector2f getRobotPosition() {
    std::lock_guard<std::mutex> lock(mMutex);
    RobotModel::Pose pose = mRobot.GetModel().getPose();
    return {pose.x, pose.y};
  }

  float getRobotOrientation() {
    std::lock_guard<std::mutex> lock(mMutex);
    RobotModel::Pose pose = mRobot.GetModel().getPose();
    return pose.theta;
  }

  /// ensure we properly break the control loop
  void requestStop() {
    std::lock_guard<std::mutex> lock(mMutex);
    mRunning = false;
    mConditionVar.notify_all();
  }

  std::vector<int> getSensorReadings() {
    std::lock_guard<std::mutex> lock(mMutex);
    //    return mSensorData.getSensorReadings();
    return {0, 0, 0, 0};
  }

  void updateSensors() {
    std::lock_guard<std::mutex> lock(mMutex);
    // Notify main thread to update sensor data
    // Example: mMaze.updateSensorData(mRobot, mSensorData);
    mConditionVar.notify_all();
  }

 private:
  Robot& mRobot;
  Maze& mMaze;
  SensorData& mSensorData;
  bool mRunning;
  std::mutex mMutex;
  std::condition_variable mConditionVar;
  ThreadSafeQueue& mLogQueue;
};

#endif  // ROBOT_CONTROL_H
