#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 */

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(600, 400), "001 Basic Window and Shapes", sf::Style::Default, settings};
  window.setFramerateLimit(60);
  // window.setPosition(sf::Vector2i(20, 20));

  /// A circle is a polygon with lots of sides
  /// if you want other plygons, set the number of sides accordingly
  static float circleRadius = 40.f;
  static std::size_t circlePoints = 18;

  sf::CircleShape circle{circleRadius, circlePoints};
  circle.setFillColor(sf::Color::Green);
  circle.setOrigin(circleRadius, circleRadius);
  circle.setPosition(100, 100);

  // Convex shapes are defined as a list of points that are rendered as triangles
  // the order of the points is important
  // create an empty shape
  sf::ConvexShape convex;

  // resize it to 5 points
  convex.setPointCount(5);

  // define the points
  convex.setPoint(0, sf::Vector2f(0, 0));
  convex.setPoint(1, sf::Vector2f(150, 10));
  convex.setPoint(2, sf::Vector2f(120, 90));
  convex.setPoint(3, sf::Vector2f(30, 100));
  convex.setPoint(4, sf::Vector2f(0, 50));
  convex.setFillColor(sf::Color::Cyan);
  convex.setOrigin(convex.getGlobalBounds().width / 2, convex.getGlobalBounds().height / 2);
  convex.setPosition(300, 100);

  /// Rectangles are simple shapes with a width and height
  sf::RectangleShape rect(sf::Vector2f(80, 20));
  rect.setFillColor(sf::Color(255, 255, 0, 192));
  rect.setOutlineColor(sf::Color(255, 0, 0, 255));
  rect.setOutlineThickness(2.f);
  rect.setPosition(60, 110);

  /// There is no class for a line as such. Instead, draw a filled rectangle
  /// if the line has thickness
  sf::RectangleShape thick_line(sf::Vector2f(150, 5));
  thick_line.setOrigin(thick_line.getSize() / 2.f);
  thick_line.setPosition(190, 200);

  /// Otherwise define the endpoints and connect the points with a dimensionless line
  sf::Vertex line[] = {sf::Vertex(sf::Vector2f(170, 210)), sf::Vertex(sf::Vector2f(150, 150))};

  sf::Clock deltaClock{};

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /// if you want items to stay the same size, you must look after resize events
      /// it can still look scrappy during the resize
      /// This works because the scene is effectively infinite and the window is just
      /// a view of it starting at (0,0)
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        /// Note that constructing a view with a sf::Rect defines the corners
        /// Using two points defines the _centre_ and the width,height
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    sf::Time time = deltaClock.restart();
    thick_line.rotate(180 * time.asSeconds());
    convex.rotate(90 * time.asSeconds());
    window.clear();
    window.draw(circle);
    window.draw(rect);
    window.draw(convex);
    window.draw(thick_line);
    window.draw(line, 2, sf::Lines);  // this is drawing a vertexlist
    window.display();
  }

  return 0;
}
