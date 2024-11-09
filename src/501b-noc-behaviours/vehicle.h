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
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(a, b);
  return dis(gen);
}

struct Vehicle {
  float v_max = 300.0f;
  float f_max = 500.0f;
  float wanderR = 75;
  float wanderD = 20;
  float change = 1.5;
  float angle = 0;

  PVector wander_circle_pos = {0, 0};
  PVector wander_target = {0, 0};
  PVector m_position;
  PVector m_velocity;
  PVector m_desired;
  PVector m_acceleration;
  PVector pursue_target;
  PVector target_prediction;
  PVector m_steering;
  PVector m_force;
  PVector m_friction;
  PVector m_target;

  Vehicle(float x, float y) { m_position = {x, y}; }

  void apply_force(PVector f) {
    m_acceleration += f;
    m_force = f;
  }

  void update(float delta_t) {
    m_friction = m_velocity;
    m_friction.set_magnitude(-100);
    //    m_acceleration += friction;
    m_velocity += m_acceleration * delta_t;
    m_velocity.limit(v_max);
    m_position += m_velocity * delta_t;
    if (m_velocity.mag() < 5) {
      m_velocity = {0, 0};
    }
    m_acceleration = {0, 0};
  }

  void seek(PVector& target) {
    m_target = target;
    m_desired = target - m_position;
    m_desired.set_magnitude(v_max);
    PVector steer = m_desired - m_velocity;
    steer.limit(f_max);
    apply_force(steer);
  }

  void flee(PVector& target) {
    m_target = target;
    m_desired = target - m_position;
    m_desired.set_magnitude(v_max);
    PVector steer = m_desired - m_velocity;
    steer.limit(f_max);
    apply_force(steer * -1);
  }

  void moveInCircle(float radius, float centerX, float centerY, float omega) {
    angle += omega;
    if (angle > 360.0f) {
      angle -= 360.0f;
    }
    if (angle < 0) {
      angle += 360.0f;
    }
    float rad = angle * (M_PI / 180.0f);
    float target_x = centerX + radius * cosf(rad);
    float target_y = centerY + radius * sinf(rad);
    PVector target = {target_x, target_y};
    seek(target);
    m_position = target;
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

  void pursue(Vehicle& vehicle) {
    pursue_target = vehicle.m_position;
    target_prediction = vehicle.m_velocity;  // * delta time?
    target_prediction *= 0.2;
    pursue_target += target_prediction;
    m_target = pursue_target;
    seek(pursue_target);
  }

  void wander() {
    float wander_theta = random(-change, change);
    wander_circle_pos = m_velocity;
    wander_circle_pos.set_magnitude(wanderD);
    wander_circle_pos += m_position;
    float h = m_velocity.angle();
    PVector c_offs(wanderR * cosf(wander_theta + h), wanderR * sinf(wander_theta + h));
    wander_target = wander_circle_pos + c_offs;
    seek(wander_target);
  }

  void check_boundaries(float x_min, float y_min, float x_max, float y_max) {
    PVector desired = {0, 0};
    float d = m_velocity.mag() / 10;
    if (m_position.x < x_min + d) {
      desired = {v_max, m_velocity.y};
    } else if (m_position.x > x_max - d) {
      desired = {-v_max, m_velocity.y};
    }
    if (m_position.y < y_min + d) {
      desired = {m_velocity.x, v_max};
    } else if (m_position.y > y_max - d) {
      desired = {m_velocity.y, -v_max};
    }
    if (desired.mag() > 0) {
      desired.normalize();
      desired *= v_max;
      PVector steer = desired - m_velocity;
      apply_force(steer);
    }
  }

  void draw(sf::RenderTarget& target) {
    float r = 10.0f;
    static float heading = 0;

    sf::CircleShape blob(r, 3);
    blob.setOrigin(r, r);
    blob.scale(0.5, 1.0);

    if (m_velocity.mag() > 1) {
      heading = 57.29 * m_velocity.angle() + 90;
    }
    blob.setRotation(heading);
    blob.setFillColor(sf::Color::Red);
    blob.setPosition(m_position.x, m_position.y);
    target.draw(blob);
    sf::CircleShape wander_circle(wanderR);
    wander_circle.setFillColor(sf::Color::Transparent);
    wander_circle.setOutlineColor(sf::Color::Yellow);
    wander_circle.setOutlineThickness(-1);
    wander_circle.setOrigin(wanderR / 2, wanderR / 2);
    wander_circle.setPosition(wander_circle_pos.x, wander_circle_pos.y);
    target.draw(wander_circle);
    sf::CircleShape wander_tgt(4);
    wander_tgt.setOrigin(2, 2);
    wander_tgt.setFillColor(sf::Color::Transparent);
    wander_tgt.setOutlineColor(sf::Color::Red);
    wander_tgt.setOutlineThickness(-1);
    wander_tgt.setPosition(wander_target.x, wander_target.y);
    target.draw(wander_tgt);
  }
};
#endif  // IMGUI_SFML_STARTER_VEHICLE_H
