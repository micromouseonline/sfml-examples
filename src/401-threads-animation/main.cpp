#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "SFML/Graphics.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"

// Shared data
std::atomic<int> shared_counter(0);
std::mutex mutex;

// Function to increment the shared counter
void increment_counter(int thread_id, int increments) {
  for (int i = 0; i < increments; ++i) {
    // Lock the mutex to ensure safe access to shared data
    std::lock_guard<std::mutex> lock(mutex);
    ++shared_counter;
    std::cout << "Thread " << thread_id << " incremented counter to " << shared_counter << std::endl;
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

/***
 * Create a simple SFML window with custom title text and a couple of shapes drawn in it
 *
 * There is a lot in here but it just covers the essentials of drawing stuff to the window.
 */

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine

  /// Windows can have Styles applied. Don't use fullscreen unless you have implemented the 'close' action below.
  //        None       = 0,      //!< No border / title bar (this flag and all others are mutually exclusive)
  //        Titlebar   = 1 << 0, //!< Title bar + fixed border
  //        Resize     = 1 << 1, //!< Title bar + resizable border + maximize button
  //        Close      = 1 << 2, //!< Title bar + close button
  //        Fullscreen = 1 << 3, //!< Fullscreen mode (this flag and all others are mutually exclusive)
  //
  //        Default = Titlebar | Resize | Close //!< Default window style
  /// Video Modes. If using fullscreen and the requested video mode is one that you monitor can be set to
  /// then it will be and, after the program exits, it will stay that way!
  sf::RenderWindow window{sf::VideoMode(1920, 1080), WINDOW_TITLE, sf::Style::Default, settings};

  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);
  /// You can set the window to a position in the monitor reference frame but this is likely not what you think
  /// The monitor reference frame is a rectangle enclosing all the monitor screens.
  //  window.setPosition(sf::Vector2i(20, 20));

  /// A circle is a polygon with lots of sides
  /// if you want other polygons, set the number of sides accordingly
  static float circleRadius = 40.f;
  static std::size_t circlePoints = 18;
  sf::CircleShape circle{circleRadius, circlePoints};
  circle.setFillColor(sf::Color::Green);
  circle.setOrigin(circleRadius, circleRadius);
  circle.setPosition(100, 100);

  /// Convex shapes are defined as a list of points that are rendered as triangles
  /// the order of the points is important as the polygon is rendered from triplets
  /// taken in order from the list.
  /// The the shape must be convex if you are to avoid strangeness
  /// create an empty shape
  sf::ConvexShape convex;
  /// resize it to 5 points
  convex.setPointCount(5);

  /// define the points
  convex.setPoint(0, sf::Vector2f(40, 0));
  convex.setPoint(1, sf::Vector2f(150, 10));
  convex.setPoint(2, sf::Vector2f(120, 90));
  convex.setPoint(3, sf::Vector2f(30, 100));
  convex.setPoint(4, sf::Vector2f(0, 50));

  /// You can fill all the triangles with the same colour together
  convex.setFillColor(sf::Color::Cyan);

  /// The origin is the point used whenever we position the object in the window
  /// it will also be the point about which rotations are centered
  /// the default origin is the top left corner of the bounding box
  /// It can be set to any location
  //  convex.setOrigin(convex.getGlobalBounds().width / 2, convex.getGlobalBounds().height / 2);
  //  convex.setOrigin(convex.getPoint(3));
  convex.setPosition(300, 100);

  /// Rectangles are simple shapes with a width and height
  /// Internally they are just two triangles
  /// By default, the origin is the top left
  sf::RectangleShape rect(sf::Vector2f(80, 20));
  /// Colours have an alpha channel. The fill here will be translucent at 50%
  rect.setFillColor(sf::Color(255, 255, 0, 128));
  rect.setOutlineColor(sf::Color::Red);
  rect.setOutlineThickness(2.f);
  rect.setPosition(60, 110);

  /// There is no class for a line as such. There are two options:
  /// For a line with finite thickness, use a rectangle
  sf::RectangleShape thick_line(sf::Vector2f(150, 5));
  thick_line.setFillColor(sf::Color(255, 0, 255, 128));
  thick_line.setOrigin(thick_line.getSize() / 2.f);
  thick_line.setPosition(190, 100);

  /// Otherwise define the endpoints and connect the points with a dimensionless line
  /// We can have several vertices and each can have a colour
  /// There are several ways to draw this vertex list
  sf::Vertex vertices[] = {
      // This is really a Vertex List
      sf::Vertex(sf::Vector2f(70, 300)),   //
      sf::Vertex(sf::Vector2f(100, 250)),  //
      sf::Vertex(sf::Vector2f(150, 250)),  //
      sf::Vertex(sf::Vector2f(180, 300)),  //
      sf::Vertex(sf::Vector2f(150, 350)),  //
      sf::Vertex(sf::Vector2f(100, 350)),  //
      sf::Vertex(sf::Vector2f(70, 300)),   //
  };
  vertices[0].color = (sf::Color::Red);
  vertices[1].color = (sf::Color::Green);
  vertices[2].color = (sf::Color::Yellow);
  vertices[3].color = (sf::Color::Blue);
  vertices[4].color = (sf::Color::Cyan);
  vertices[5].color = (sf::Color::Green);
  vertices[6].color = (sf::Color::Red);

  /// These are the ways you can use a vertex list to draw shapes
  /// Triangles have their vertex colours interpolated across the triangle
  sf::Vertex lines[7];
  sf::Vertex line_strip[7];
  sf::Vertex triangles[7];
  sf::Vertex triangle_strip[7];
  sf::Vertex triangle_fan[7];

  for (int i = 0; i < 7; i++) {
    lines[i] = vertices[i];
    line_strip[i] = vertices[i];
    triangles[i] = vertices[i];
    triangle_strip[i] = vertices[i];
    triangle_fan[i] = vertices[i];
    line_strip[i].position.x += 150;
    triangles[i].position.x += 300;
    triangle_strip[i].position.x += 450;
    triangle_fan[i].position.x += 600;
  }
  sf::Clock deltaClock{};  /// Keeps track of elapsed time

  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      /// This is how you listen for the window close button being pressed
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      /// this is how you listen for a specific key being pressed
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.scancode == sf::Keyboard::Scancode::Escape) {
          window.close();
        }
      }
      /// if you want items to stay the same size, you must look after resize events
      /// it can still look scrappy during the resize
      /// This works because the scene is effectively infinite and the window is just
      /// a view of it starting at (0,0)
      if (event.type == sf::Event::Resized) {
        sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));  // or everything distorts
      }
    }
    /// When a Clock is restarted, the current elapsed time is returned.
    /// restart the clock once per frame to get the frame refresh interval
    sf::Time time = deltaClock.restart();
    thick_line.rotate(36 * time.asSeconds());  // 36 degrees per second
    convex.rotate(90 * time.asSeconds());      // 90 degrees per second

    /// Only after updating everything should you redraw the window.
    /// Just do the drawing in this section.
    /// Drawing is done to an off screen buffer in the GPU.
    /// always start from a clean slate
    window.clear();
    window.draw(circle);
    window.draw(rect);
    window.draw(convex);
    window.draw(thick_line);
    /// Various ways to render a list of vertices. Pick one
    window.draw(lines, 6, sf::Lines);
    window.draw(line_strip, 7, sf::LineStrip);  /// note the extra vertex to close the shape
    window.draw(triangles, 6, sf::Triangles);
    window.draw(triangle_strip, 6, sf::TriangleStrip);
    window.draw(triangle_fan, 6, sf::TriangleFan);
    /// This makes the off-screen buffer live and will wait for the refresh event
    /// specified above. Normally a simple wait fo elapsed time is used
    window.display();
  }

  return 0;
}
