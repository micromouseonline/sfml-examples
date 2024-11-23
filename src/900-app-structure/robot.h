//
// Created by peter on 22/11/24.
//

#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include "collision-object.h"
#include "robot-model.h"
#include "sensor.h"

/***
 * this is the main robot class.
 * Is it an unnecessary indirection?
 * Could/should all the robot behaviour be in one class?
 *
 * The robot will have the sensors and  the collision geometry
 */
class Robot {
 public:
  Robot() {
    // create the robot
  }

  RobotModel& GetModel() { return m_model; }

 private:
  RobotModel m_model;
  CollisionGeometry m_robot_body;
};
#endif  // ROBOT_H
