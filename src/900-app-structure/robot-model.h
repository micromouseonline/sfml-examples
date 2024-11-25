//
// Created by peter on 02/11/2024.
//

#ifndef ROBOT_MODEL_H
#define ROBOT_MODEL_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "core.h"
/***
 * The RobotModel class is the model. It handles the physical properties and  behaviour of
 * the robot hardware - the pose, and dynamics.
 *
 * It does not handle sensors, odometry and such. That is the job of the Robot.
 *
 *Key Responsibilities for RobotModel
 * Physical Properties:
 *    - Pose: Position (x, y) and orientation (θ) of the robot in the simulation world.
 *    - Dynamics: State of the robot, including velocity (linear and angular) and possibly acceleration.
 *      (acceleration s not modelled here)
 *
 * Behavior:
 *    - Updates to the pose based on velocity inputs.
 *    - Handling dynamics (e.g., ensuring realistic acceleration/deceleration, if modeled).
 *    - Applying kinematic or dynamic constraints (e.g., for differential drive or holonomic robots).
 *
 * Odometry:
 *    - Estimation of position and orientation based on wheel encoder data or similar sensors.
 *    - This simple model just assumes everything corresponds to demanded positions
 *
 */

////////////////////////////////////////////////////////////////////////////////////
class RobotModel {
 public:
  struct Pose {
    float x;      // mm
    float y;      // mm
    float theta;  // degrees
  };

  struct Velocity {
    float linear;   // mm/s
    float angular;  // deg/s
  };

  RobotModel() : m_pose({0, 0, 0}), m_velocity({0, 0}) {
    //
  }

  RobotModel(float x, float y, float theta = 0)
      : m_pose({x, y, theta}),
        m_velocity({0, 0}){
            //
        };

  // Getters
  [[nodiscard]] RobotModel::Pose getPose() const {
    return m_pose;  //
  }
  [[nodiscard]] RobotModel::Velocity getVelocity() const {
    return m_velocity;  //
  }

  // Set input velocities
  void SetVelocities(double linear, double angular) {
    m_velocity.linear = linear;
    m_velocity.angular = angular;
  }

  // Helper: Compute next pose based on current velocity
  void Update(float deltaTime) {
    m_pose.theta += m_velocity.angular * deltaTime;
    if (m_pose.theta > 360.0f) {
      m_pose.theta -= 360.0f;
    }
    if (m_pose.theta < -360.0f) {
      m_pose.theta += 2 * 360.0f;
    }
    float ds = m_velocity.linear * deltaTime;
    float dx = ds * cosf((m_pose.theta - 90.0f) * (float)M_PI / 180.0f);
    float dy = ds * sinf((m_pose.theta - 90.0f) * (float)M_PI / 180.0f);
    m_pose.x += dx;
    m_pose.y += dy;
  }

  void SetSpeed(float speed) {
    m_velocity.linear = speed;  // mm/s
  }
  void SetOmega(float omega) {
    m_velocity.angular = omega;  // deg/s
  }

  ////////

  // Move directly
  void SetPosition(float x, float y) {
    m_pose.x = x;
    m_pose.y = y;
  }

  void SetAngle(float angle) {
    m_pose.theta = angle;  // degrees
  }

  void Move(float dx, float dy) {
    m_pose.x += dx;
    m_pose.y += dy;
  }
  void Move(sf::Vector2f direction) { Move(direction.x, direction.y); }

  void Rotate(float angle) {
    m_pose.theta += angle;  // degrees
  }

  ///////////////////

 private:
  Pose m_pose;
  Velocity m_velocity;
};

#endif  // ROBOT_MODEL_H
