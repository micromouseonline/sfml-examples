#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>

class Robot {
 public:
  Robot() : velocity(0), omega(0), position(0, 0), distance(0), orientation(0) {}

  void update(float deltaTime) {
    distance += velocity * deltaTime;
    position.x += velocity * std::cos(orientation) * deltaTime;
    position.y += velocity * std::sin(orientation) * deltaTime;
    orientation += omega * deltaTime;
  }

  void setSpeeds(float v, float w) {
    velocity = v;
    omega = w;
  }

  float velocity;
  float omega;
  sf::Vector2f position;
  float distance;
  float orientation;

 private:
};

class Behavior {
 public:
  Behavior(Robot& robot) : m_robot(robot) {}

  void requestMove(float distance, float maxSpeed, float finalSpeed, float acceleration) {
    m_profile.initProfile(distance, maxSpeed, finalSpeed, acceleration);
  }

  void update(float deltaTime) {
    float currentSpeed = m_profile.update(deltaTime);
    m_robot.setSpeeds(currentSpeed, 0);
  }

  Robot& m_robot;
  Profile m_profile;

 private:
};
