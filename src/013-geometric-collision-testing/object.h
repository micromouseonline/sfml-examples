#ifndef _OBJECT_H
#define _OBJECT_H

#include <SFML/Graphics.hpp>
#include <cmath>

class ComplexObject {
 public:
  ComplexObject(const sf::Vector2f& center) : m_center(center) {}

  // Add a shape to the object
  void addShape(std::unique_ptr<sf::Shape> shape, const sf::Vector2f& offset) {
    //    shape->setOrigin(shape->getLocalBounds().width / 2, shape->getLocalBounds().height / 2);
    //    shape->setOrigin(m_center.x + offset.x, m_center.y + offset.y);
    shapes.push_back(std::make_pair(std::move(shape), offset));
  }

  void setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }
  void setPosition(const sf::Vector2f& position) {
    m_center = position;
    for (auto& shapePair : shapes) {
      auto& shape = shapePair.first;
      auto& initialOffset = shapePair.second;
      shape->setPosition(m_center + initialOffset);
    }
  }
  sf::Vector2f getPosition() { return m_center; }

  // Rotate the object around its center
  void rotate(float angle) {
    float radAngle = angle * (3.14159265359f / 180.f);
    float cosAngle = std::cos(radAngle);
    float sinAngle = std::sin(radAngle);

    for (auto& shapePair : shapes) {
      auto& shape = shapePair.first;
      auto& initialOffset = shapePair.second;

      // Calculate the new position after rotation
      sf::Vector2f rotatedOffset;
      rotatedOffset.x = initialOffset.x * cosAngle - initialOffset.y * sinAngle;
      rotatedOffset.y = initialOffset.x * sinAngle + initialOffset.y * cosAngle;

      shape->setPosition(m_center + rotatedOffset);
      shape->setRotation(shape->getRotation() + angle);
    }
  }

  // Draw all shapes
  void draw(sf::RenderWindow& window) {
    for (const auto& shapePair : shapes) {
      window.draw(*shapePair.first);
    }
  }

 private:
  sf::Vector2f m_center;
  std::vector<std::pair<std::unique_ptr<sf::Shape>, sf::Vector2f>> shapes;
};

#endif  // _OBJECT_H