//
// Created by peter on 07/11/24.
//

#ifndef IMGUI_SFML_STARTER_PVECTOR_H
#define IMGUI_SFML_STARTER_PVECTOR_H

#include <cmath>
#include <iostream>
#include <random>

class PVector {
 public:
  float x;
  float y;

  // Constructors
  PVector(float x = 0, float y = 0) : x(x), y(y) {}

  PVector from_angle(float angle) { return {cosf(angle), sinf(angle)}; }

  PVector random() {
    float r = (float)rand() / (float)RAND_MAX;
    return from_angle(r * 2 * M_PI);
  }

  // Add two vectors
  PVector operator+(const PVector& v) const { return PVector(x + v.x, y + v.y); }

  // Subtract two vectors
  PVector operator-(const PVector& v) const { return PVector(x - v.x, y - v.y); }

  // Multiply vector by a scalar
  PVector operator*(float n) const { return PVector(x * n, y * n); }

  // Divide vector by a scalar
  PVector operator/(float n) const {
    if (n != 0) {
      return PVector(x / n, y / n);
    } else {
      std::cerr << "Error: Division by zero" << std::endl;
      return PVector();
    }
  }

  // Compound assignment operators
  PVector& operator+=(const PVector& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  PVector& operator-=(const PVector& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  PVector& operator*=(float n) {
    x *= n;
    y *= n;
    return *this;
  }

  PVector& operator/=(float n) {
    if (n != 0) {
      x /= n;
      y /= n;
    } else {
      std::cerr << "Error: Division by zero" << std::endl;
    }
    return *this;
  }

  // Calculate the magnitude of the vector
  float mag() const { return std::sqrt(x * x + y * y); }

  // Normalize the vector to a unit vector
  PVector& normalize() {
    float m = mag();
    if (m != 0) {
      *this /= m;
    }
    return *this;
  }

  PVector& set_magnitude(float m) {
    normalize();
    *this *= m;
    return *this;
  }

  PVector& limit(float lim) {
    float m = mag();
    if (m > lim) {
      *this /= m;
      *this *= lim;
    }
    return *this;
  }

  // Comparison operators based on magnitude
  bool operator==(const PVector& v) const { return mag() == v.mag(); }
  bool operator!=(const PVector& v) const { return mag() != v.mag(); }
  bool operator<(const PVector& v) const { return mag() < v.mag(); }
  bool operator<=(const PVector& v) const { return mag() <= v.mag(); }
  bool operator>(const PVector& v) const { return mag() > v.mag(); }
  bool operator>=(const PVector& v) const { return mag() >= v.mag(); }

  // Calculate the dot product of two vectors
  float dot(const PVector& v) const { return x * v.x + y * v.y; }

  float angle() {
    static float hdg = 0;
    if (mag() > 1) {
      hdg = atan2f(y, x);
    }
    return hdg;
  }

  float angle_to(PVector& v2) { return acosf(dot(v2)) / mag() / v2.mag(); }

  PVector& rotate_to(float theta) {
    float m = mag();
    x = m * cosf(theta);
    y = m * sinf(theta);
    return *this;
  }

  PVector& rotate_by(float theta) {
    rotate_to(angle() + theta);
    return *this;
  }

  void print() const { std::cout << "PVector(" << x << ", " << y << ")" << std::endl; }
};

#endif  // IMGUI_SFML_STARTER_PVECTOR_H
