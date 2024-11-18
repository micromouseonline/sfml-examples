//
// Created by peter on 18/11/24.
//

#ifndef IMGUI_SFML_STARTER_VEC2_H
#define IMGUI_SFML_STARTER_VEC2_H

#include <cmath>

struct Vec2 {
  Vec2() : x(0.0f), y(0.0f) {}

  Vec2(float x_, float y_) : x(x_), y(y_) {}

  float x, y;

  void operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;
  }

  void operator-=(const Vec2& v) {
    x -= v.x;
    y -= v.y;
  }

  float length() const { return sqrt(x * x + y * y); }

  Vec2 getNormalized() const {
    const float len = length();
    return Vec2(x / len, y / len);
  }

  Vec2 normal() const { return Vec2(-y, x); }
};

inline Vec2 operator+(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2) {
  return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline Vec2 operator*(const Vec2& v, float f) {
  return Vec2(v.x * f, v.y * f);
}

inline float dot(const Vec2& v1, const Vec2& v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

inline float getDistance(const Vec2& v1, const Vec2& v2) {
  return (v1 - v2).length();
}

// line is from v-wchar_t
// point is p
inline float minimum_distance(Vec2 v, Vec2 w, Vec2 p) {
  // Return minimum distance between line segment vw and point p
  Vec2 line = w - v;
  const float l2 = line.length();  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0)
    return getDistance(p, v);  // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  const float t = std::max(0.0f, std::min(1.0f, dot(p - v, w - v) / l2));
  const Vec2 projection = v + line * t;  // Projection falls on the segment
  return getDistance(p, projection);
}

#endif  // IMGUI_SFML_STARTER_VEC2_H
