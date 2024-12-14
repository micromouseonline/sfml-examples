#pragma once

#include <algorithm>
#include <cmath>

class Profile {
 public:
  enum Phase { Acceleration, Cruise, Deceleration, Complete };
  Profile() : distance(0), maxSpeed(0), finalSpeed(0), acceleration(0), currentSpeed(0), currentPosition(0), phase(Acceleration) {}

  void initProfile(float dist, float v_max, float v_end, float acc) {
    this->distance = dist;
    this->maxSpeed = v_max;
    this->finalSpeed = v_end;
    this->acceleration = acc;
    this->currentSpeed = 0;
    this->currentPosition = 0;
    this->phase = Acceleration;
    // Calculate distances for each phase
    accelDistance = (maxSpeed * maxSpeed) / (2 * acceleration);
    decelDistance = (maxSpeed * maxSpeed - finalSpeed * finalSpeed) / (2 * acceleration);
    cruiseDistance = std::max(0.0f, distance - accelDistance - decelDistance);  // Check if the distance is too short to reach the maximum speed
    if (distance < accelDistance + decelDistance) {
      // Calculate new acceleration and deceleration distances
      float adjustedSpeed = std::sqrt((2 * acceleration * distance + finalSpeed * finalSpeed) / 2);
      accelDistance = (adjustedSpeed * adjustedSpeed - finalSpeed * finalSpeed) / (2 * acceleration);
      decelDistance = accelDistance;
      cruiseDistance = 0;
    }
  }

  float update(float deltaTime) {
    switch (phase) {
      case Acceleration:
        currentSpeed += acceleration * deltaTime;
        currentSpeed = std::min(currentSpeed, maxSpeed);
        currentPosition += currentSpeed * deltaTime;
        if (currentPosition >= accelDistance) {
          phase = Cruise;
        }
        break;
      case Cruise:
        currentPosition += maxSpeed * deltaTime;
        if (currentPosition >= accelDistance + cruiseDistance) {
          phase = Deceleration;
        }
        break;
      case Deceleration:
        currentSpeed -= acceleration * deltaTime;
        currentSpeed = std::max(currentSpeed, finalSpeed);
        currentPosition += currentSpeed * deltaTime;
        if (currentPosition >= distance) {
          phase = Complete;
          currentSpeed = finalSpeed;
        }
        break;
      case Complete:
        // No more movement needed
        break;
    }
    return currentSpeed;
  }

  Phase getPhase() { return phase; }

 private:
  float distance;
  float maxSpeed;
  float finalSpeed;
  float acceleration;
  float currentSpeed;
  float currentPosition;
  float accelDistance;
  float cruiseDistance;
  float decelDistance;
  Phase phase;
};
