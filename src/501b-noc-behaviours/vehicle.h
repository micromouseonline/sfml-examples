//
// Created by peter on 08/11/24.
//

#ifndef IMGUI_SFML_STARTER_VEHICLE_H
#define IMGUI_SFML_STARTER_VEHICLE_H

#include <random>

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float random(float a, float b) {
  static std::random_device rd;  //  make this globally available
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(a, b);
  return dis(gen);
}

struct Vehicle {
  float v_max = 1400.0f;
  float f_max = 900.0f;
  float wanderR = 75;
  float wanderD = 20;
  float change = 1.5;
  float angle = 0;

  static constexpr float EPSILON = 1e-6;
  const float panic_distance = 400;

  PVector wander_circle_pos = PVector(0, 0);
  PVector wander_target = PVector(0, 0);
  PVector m_position;
  PVector m_velocity;
  PVector m_desired;
  PVector m_acceleration;
  PVector pursue_target;
  PVector target_prediction;
  PVector m_steering;
  PVector m_force;
  PVector m_target;

  Vehicle(float x, float y) : m_position(x, y) {}

  void apply_force(PVector f) {
    m_acceleration += f;
    m_force = f;
  }

  void update(float delta_t) {
    m_velocity += m_acceleration * delta_t;
    m_velocity.limit(v_max);
    m_position += m_velocity * delta_t;
    m_acceleration *= 0.1;
  }

  void seek(PVector& target) {
    if (target == m_position) {
      return;
    }
    m_target = target;

    m_desired = target - m_position;
    if (m_desired.mag() < 1) {
      return;
    }
    m_desired.set_magnitude(v_max);
    PVector steer = m_desired - m_velocity;
    steer.limit(f_max);
    apply_force(steer);
  }

  void flee(PVector& target) {
    m_target = target;
    m_desired = target - m_position;
    if (m_desired.mag() > panic_distance)
      return;
    m_desired.set_magnitude(v_max);
    PVector steer = m_desired - m_velocity;
    steer.limit(f_max);
    apply_force(steer * -1);
  }

  void arrive(PVector& target) {
    m_target = target;
    m_desired = target - m_position;
    float d = m_desired.mag();
    if (d < 1) {
      return;
    }
    float range = 100;
    if (d < range) {
      float m = map(d, 0, range, 0, v_max);
      m_desired.set_magnitude(m);
    } else {
      m_desired.set_magnitude(v_max);
    }
    PVector steer = m_desired - m_velocity;
    steer.limit(f_max);
    apply_force(steer);
  }

  void moveInCircle(float radius, float centerX, float centerY, float omega, float dt) {
    angle += omega * dt;
    angle = fmod(angle, 360.0f);

    float rad = angle * (M_PI / 180.0f);
    float target_x = centerX + radius * cosf(rad);
    float target_y = centerY + radius * sinf(rad);
    PVector target(target_x, target_y);
    seek(target);
    m_position = target;
    omega /= 57.29;
    float vx = -radius * omega * sinf(rad);
    float vy = radius * omega * cosf(rad);
    m_velocity = PVector(vx, vy);
  }

  void pursue(Vehicle& vehicle) {
    pursue_target = vehicle.m_position;
    target_prediction = vehicle.m_velocity;  // * delta time?
    target_prediction *= 0.2;
    pursue_target += target_prediction;
    m_target = pursue_target;
    arrive(pursue_target);
  }

  void wander() {
    float wander_theta = random(-change, change);
    PVector velocity = m_velocity;
    velocity.normalize();
    wander_circle_pos = velocity;
    wander_circle_pos.set_magnitude(wanderD);
    wander_circle_pos += m_position;
    float h = velocity.angle();
    PVector c_offs(wanderR * cosf(wander_theta + h), wanderR * sinf(wander_theta + h));
    wander_target = wander_circle_pos + c_offs;
    seek(wander_target);
  }

  void check_boundaries(float x_min, float y_min, float x_max, float y_max) {
    float d = m_velocity.mag() / 10;
    if (m_position.x < x_min + d || m_position.x > x_max - d || m_position.y < y_min + d || m_position.y > y_max - d) {
      PVector desired;
      d = 20;
      if (m_position.x < x_min + d) {
        desired = PVector(v_max, m_velocity.y);
      } else if (m_position.x > x_max - d) {
        desired = PVector(-v_max, m_velocity.y);
      }
      if (m_position.y < y_min + d) {
        desired = PVector(m_velocity.x, v_max);
      } else if (m_position.y > y_max - d) {
        desired = PVector(m_velocity.x, -v_max);
      }
      if (desired.mag() > 0) {
        desired.normalize();
        desired *= v_max;
        PVector steer = desired - m_velocity;
        steer.limit(f_max);
        apply_force(steer);
      }
    }
  }
};

#endif  // IMGUI_SFML_STARTER_VEHICLE_H
