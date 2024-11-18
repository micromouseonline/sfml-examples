#ifndef _OBJECT_H
#define _OBJECT_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "collisions.h"

class ComplexObject {
 public:
  ComplexObject(const sf::Vector2f& center) : m_center(center) {}

  // Add a shape to the object
  void addShape(std::unique_ptr<sf::Shape> shape, const sf::Vector2f& offset) { shapes.push_back(std::move(shape)); }

  void setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }
  void setPosition(const sf::Vector2f& position) {
    m_center = position;
    for (auto& shape : shapes) {
      shape->setPosition(m_center);
    }
  }
  sf::Vector2f getPosition() { return m_center; }

  void rotate(float angle) {
    /// Each shape has its origin set to the relative center of the object
    for (auto& shape : shapes) {
      shape->setRotation(shape->getRotation() + angle);
    }
  }

  // Draw all shapes
  void draw(sf::RenderWindow& window) {
    for (const auto& shape : shapes) {
      window.draw(*shape);
    }
  }

  sf::Vector2f circle_centre() { return cpos; }
  sf::Vector2f circle_origin() { return corigin; }

  bool collides_with(const sf::RectangleShape& rect) {
    for (const auto& shape : shapes) {
      if (sf::CircleShape* circle = dynamic_cast<sf::CircleShape*>(shape.get())) {
        cx = circle->getPosition().x;
        cy = circle->getPosition().y;
        cpos = circle->getPosition() - sf::Vector2f(0, circle->getRadius());
        corigin = circle->getOrigin();
        auto test_circle = sf::CircleShape(*circle);
        test_circle.move({0, -circle->getOrigin().y - circle->getRadius()});
        cpos = test_circle.getPosition();
        if (isCircleOverlappingRectangle(rect, test_circle)) {
          return true;
        }
      } else if (auto this_rect = dynamic_cast<sf::RectangleShape*>(shape.get())) {
        if (isOverlapping(rect, *this_rect)) {
          //          return true;
        }
      }
    }
    return false;
  }

 private:
  float cx;
  float cy;
  sf::Vector2f cpos;
  sf::Vector2f corigin;
  sf::Vector2f m_center;
  std::vector<std::unique_ptr<sf::Shape>> shapes;
};

#endif  // _OBJECT_H