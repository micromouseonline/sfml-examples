//
// Created by peter on 18/11/24.
//

#ifndef IMGUI_SFML_STARTER_COLLISIONS_H
#define IMGUI_SFML_STARTER_COLLISIONS_H

bool isCircleOverlappingRectangle(const sf::RectangleShape& rect, const sf::CircleShape& circle) {
  // Adjust for the circle's origin offset
  sf::Vector2f circleCenter;
  circleCenter.x = circle.getPosition().x;                         //+ circle.getOrigin().x;
  circleCenter.y = circle.getPosition().y + circle.getOrigin().y;  // circleCenter = circle.getPosition()   - circle.getOrigin();

  // Find the closest point to the circle within the rectangle
  float closestX = std::clamp(circleCenter.x, rect.getPosition().x, rect.getPosition().x + rect.getSize().x);
  float closestY = std::clamp(circleCenter.y, rect.getPosition().y, rect.getPosition().y + rect.getSize().y);

  // Calculate the distance between the circle's center and this closest point
  float distanceX = circleCenter.x - closestX;
  float distanceY = circleCenter.y - closestY;

  // Calculate the distance squared and compare with the radius squared
  float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
  return distanceSquared < (circle.getRadius() * circle.getRadius());
}

/////////////////////////////////////////////////////////////////////

// Function to project rectangle onto an axis
std::pair<float, float> projectRectangle(const sf::RectangleShape& rect, const sf::Vector2f& axis) {
  std::vector<sf::Vector2f> points(4);
  for (size_t i = 0; i < 4; ++i) {
    points[i] = rect.getTransform().transformPoint(rect.getPoint(i));
  }
  float min = (points[0].x * axis.x + points[0].y * axis.y);
  float max = min;
  for (size_t i = 1; i < 4; ++i) {
    float projection = (points[i].x * axis.x + points[i].y * axis.y);
    if (projection < min)
      min = projection;
    if (projection > max)
      max = projection;
  }
  return std::make_pair(min, max);
}

// Function to check for separating axis
bool isSeparatingAxis(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2, const sf::Vector2f& axis) {
  auto projection1 = projectRectangle(rect1, axis);
  auto projection2 = projectRectangle(rect2, axis);

  return (projection1.second < projection2.first || projection2.second < projection1.first);
}

// Function to check for overlap
bool isOverlapping(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2) {
  std::vector<sf::Vector2f> axes = {rect1.getTransform().transformPoint(rect1.getPoint(1)) - rect1.getTransform().transformPoint(rect1.getPoint(0)),
                                    rect1.getTransform().transformPoint(rect1.getPoint(3)) - rect1.getTransform().transformPoint(rect1.getPoint(0)),
                                    rect2.getTransform().transformPoint(rect2.getPoint(1)) - rect2.getTransform().transformPoint(rect2.getPoint(0)),
                                    rect2.getTransform().transformPoint(rect2.getPoint(3)) - rect2.getTransform().transformPoint(rect2.getPoint(0))};

  for (auto& axis : axes) {
    if (isSeparatingAxis(rect1, rect2, sf::Vector2f(-axis.y, axis.x))) {
      return false;  // There is a separating axis, so they do not overlap
    }
  }
  return true;  // No separating axis found, so they overlap
}

/////////////////////////////////////////////////////////////////////

// Function to project a point onto an axis
float projectPoint(const sf::Vector2f& point, const sf::Vector2f& axis) {
  return (point.x * axis.x + point.y * axis.y);
}

// Function to get the closest point on the rectangle to the circle center
sf::Vector2f getClosestPointOnRectangle(const sf::RectangleShape& rect, const sf::Vector2f& circleCenter) {
  sf::Vector2f closestPoint = circleCenter;
  std::vector<sf::Vector2f> vertices(4);
  for (size_t i = 0; i < 4; ++i) {
    vertices[i] = rect.getTransform().transformPoint(rect.getPoint(i));
  }

  for (const auto& axis : {sf::Vector2f(1, 0), sf::Vector2f(0, 1)}) {
    float minDistance = std::numeric_limits<float>::max();
    for (const auto& vertex : vertices) {
      float distance = std::abs(projectPoint(vertex - circleCenter, axis));
      if (distance < minDistance) {
        minDistance = distance;
        if (axis.x == 1) {
          closestPoint.x = vertex.x;
        } else {
          closestPoint.y = vertex.y;
        }
      }
    }
  }
  return closestPoint;
}

// Function to check for overlap
bool isCircleOverlappingRotatedRectangle(const sf::CircleShape& circle, const sf::RectangleShape& rect) {
  // Get circle properties
  sf::Vector2f circleCenter = circle.getPosition() + sf::Vector2f(circle.getRadius(), circle.getRadius());
  float circleRadius = circle.getRadius();

  // Get the closest point on the rectangle to the circle center
  sf::Vector2f closestPoint = getClosestPointOnRectangle(rect, circleCenter);

  // Calculate the distance between the closest point and the circle center
  float distanceX = circleCenter.x - closestPoint.x;
  float distanceY = circleCenter.y - closestPoint.y;
  float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

  return distanceSquared < (circleRadius * circleRadius);
}

#endif  // IMGUI_SFML_STARTER_COLLISIONS_H
