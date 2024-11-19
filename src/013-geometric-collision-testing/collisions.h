//
// Created by peter on 18/11/24.
//

#ifndef COLLISIONS_H
#define COLLISIONS_H

namespace Collisions {

/// This is a test only valid for axis aligned rectangles
bool circle_hits_rect(const sf::CircleShape& circle, const sf::RectangleShape& rect) {
  const sf::Vector2f circleCenter = circle.getPosition();

  const auto& rectBounds = rect.getGlobalBounds();
  const float rectLeft = rectBounds.left;
  const float rectTop = rectBounds.top;
  const float rectRight = rectBounds.left + rectBounds.width;
  const float rectBottom = rectTop + rectBounds.height;

  // Clamp the circle's center coordinates to the rectangle's edges
  const float closestX = std::clamp(circleCenter.x, rectLeft, rectRight);
  const float closestY = std::clamp(circleCenter.y, rectTop, rectBottom);

  // Calculate the squared distance from the circle's center to the closest point
  const float deltaX = circleCenter.x - closestX;
  const float deltaY = circleCenter.y - closestY;
  const float distanceSquared = deltaX * deltaX + deltaY * deltaY;

  // Compare squared distances to avoid costly sqrt operation
  const float radius = circle.getRadius();
  return distanceSquared < (radius * radius);
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
bool rectangles_overlap(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2) {
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

// Function to project a vector onto another vector (axis)
float project_vector(const sf::Vector2f& vector, const sf::Vector2f& axis) {
  // Normalize the axis to ensure a proper projection
  float axisLengthSquared = axis.x * axis.x + axis.y * axis.y;
  if (axisLengthSquared == 0.0f) {
    std::cerr << "Axis vector cannot be zero in project_vector.\n";
    return std::numeric_limits<float>::infinity();
  }
  return (vector.x * axis.x + vector.y * axis.y) / std::sqrt(axisLengthSquared);
}

// Helper: Get rectangle vertices in world space
std::vector<sf::Vector2f> getRectangleVertices(const sf::RectangleShape& rect) {
  std::vector<sf::Vector2f> vertices(4);
  for (size_t i = 0; i < 4; ++i) {
    vertices[i] = rect.getTransform().transformPoint(rect.getPoint(i));
  }
  return vertices;
}

/// This more generalise case is computationally more expensive than the axis-aligned case at the top
bool circle_hits_rotated_rect(const sf::CircleShape& circle, const sf::RectangleShape& rect) {
  // Circle properties
  sf::Vector2f circleCenter = circle.getPosition();
  float circleRadius = circle.getRadius();

  // Get rectangle vertices and edges
  auto vertices = getRectangleVertices(rect);
  std::vector<sf::Vector2f> edges = {vertices[1] - vertices[0], vertices[2] - vertices[1], vertices[3] - vertices[2], vertices[0] - vertices[3]};

  // Add the circle's center-to-vertex axes for SAT
  for (const auto& vertex : vertices) {
    edges.push_back(circleCenter - vertex);
  }

  // Check for overlap on all axes
  for (const auto& edge : edges) {
    sf::Vector2f axis(-edge.y, edge.x);  // Perpendicular axis

    // Project circle onto the axis
    float circleProjection = project_vector(circleCenter, axis);
    float circleMin = circleProjection - circleRadius;
    float circleMax = circleProjection + circleRadius;

    // Project rectangle onto the axis
    float rectMin = std::numeric_limits<float>::max();
    float rectMax = std::numeric_limits<float>::lowest();
    for (const auto& vertex : vertices) {
      float projection = project_vector(vertex, axis);
      rectMin = std::min(rectMin, projection);
      rectMax = std::max(rectMax, projection);
    }

    // Check for overlap
    if (circleMax < rectMin || circleMin > rectMax) {
      return false;  // No overlap on this axis, so no collision
    }
  }

  return true;  // Overlap on all axes, collision detected
}
/////////////////////////////////////////////////////////////////////
bool point_hits_rect(sf::Vector2f& p, sf::RectangleShape& rect) {
  return rect.getGlobalBounds().contains(p);
}

}  // namespace Collisions

#endif  // COLLISIONS_H
