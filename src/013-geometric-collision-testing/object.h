#ifndef _OBJECT_H
#define _OBJECT_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <memory>
#include <vector>
#include "collisions.h"

class ComplexObject {
 public:
  ComplexObject(const sf::Vector2f& center) : m_center(center) {}

  void addShape(std::unique_ptr<sf::Shape> shape, const sf::Vector2f& offset) {
    shape->setPosition(m_center + offset);
    shapes.emplace_back(std::move(shape), offset);
  }

  void rotate(float angle) {
    // Convert angle from degrees to radians
    float radAngle = angle * (3.14159265359f / 180.f);
    float cosAngle = std::cos(radAngle);
    float sinAngle = std::sin(radAngle);

    for (auto& shapePair : shapes) {
      auto& shape = shapePair.first;
      auto& initialOffset = shapePair.second;

      // Step 1: Rotate the offset vector around m_center
      sf::Vector2f rotatedOffset;
      rotatedOffset.x = initialOffset.x * cosAngle - initialOffset.y * sinAngle;
      rotatedOffset.y = initialOffset.x * sinAngle + initialOffset.y * cosAngle;

      // Step 2: Set the position of the shape relative to m_center
      initialOffset = rotatedOffset;
      shape->setPosition(m_center + rotatedOffset);

      // Step 3: Rotate the shape around its own local origin
      shape->setRotation(shape->getRotation() + angle);
    }
  }

  void setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }

  void setPosition(const sf::Vector2f& position) { m_center = position; }

  sf::Vector2f getPosition() const { return m_center; }

  void draw(sf::RenderWindow& window) {
    for (const auto& shapePair : shapes) {
      window.draw(*shapePair.first);
    }
  }

  bool collides_with(const sf::RectangleShape& rect) {
    for (const auto& shapePair : shapes) {
      auto& shape = *shapePair.first;
      if (auto* circle = dynamic_cast<sf::CircleShape*>(&shape)) {
        if (isCircleOverlappingRectangle(rect, *circle))
          return true;
      } else if (auto* this_rect = dynamic_cast<sf::RectangleShape*>(&shape)) {
        if (isOverlapping(rect, *this_rect))
          return true;
      }
    }
    return false;
  }

 private:
  sf::Vector2f m_center;
  std::vector<std::pair<std::unique_ptr<sf::Shape>, sf::Vector2f>> shapes;
  std::vector<std::pair<std::unique_ptr<sf::Shape>, sf::Vector2f>> originalOffsets;
};

#endif  // _OBJECT_H
