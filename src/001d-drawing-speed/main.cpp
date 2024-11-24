#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

/**
 * In this experiment we draw a bunch of rectangles to the screen and time the operations.
 *
 * A sf::RectangleShape is a rectangle with a position, size, and information about
 * colors and textures. It is stored in RAM as a vertex array of four points,
 * which are used for rendering the rectangle on the screen.
 *
 * A sf::Rect is a simple structure with a position and size. It is stored in RAM.
 *
 * To draw an sf::RectangleShape, a separate call is made to the GPU for each shape,
 * sending the four vertices that define the rectangle. After that, the GPU processes
 * and renders the shape accordingly.
 *
 * To draw an sf::Rect, we must first create an sf::RectangleShape from the sf::Rect and then
 * draw the sf::RectangleShape using the GPU.
 *
 * It should be clear that, if we have a lot of rectangles to draw, the sf::VertexArray is
 * faster than drawing multiple sf::RectangleShapes individually.
 *
 * Since each call to the GPU takes time, it is much more efficient to create a larger
 * vertex array describing all the vertices in one structure and send that to the GPU.
 *
 * A sf::VertexArray is a flexible array of vertices that define shapes, lines, or points
 * with specific positions, colors, and texture coordinates. It is stored in RAM and is
 * used for efficient rendering of complex shapes and multiple primitives in a single draw call.
 *
 * We can treat a continuous array of vertices as if they were a group of four vertices - just
 * like having a list of sf::RectangleShapes. Except that now, we send them all to the GPU in
 * a single call which is very quick.
 *
 * To draw 2000 individual sf::RectangleShapes, we use 8000 vertices in 2000 calls. That might
 * take 2000us on a particular machine. Converting the rectangles to a single list of vertices
 * and sending it to the GPU in one call takes only 30us on the same machine.
 *
 * The process of converting the 2000 individual sf::RectangleShapes into a single sf::VertexArray
 * might take about 300us.
 *
 * So, to draw 2000 sf::RectangleShapes, it is much faster to convert them into a single structure.
 * Even if we include the time needed to do the conversion, it might still be ten times faster overall.
 */

const int ShapeCount = 2000;
std::vector<sf::RectangleShape> shapes;
std::vector<sf::Rect<float>> rectangles;

sf::VertexArray createVertexArrayFromRects(const std::vector<sf::Rect<float>>& rects, const sf::Color& color = sf::Color::Magenta) {
  sf::VertexArray vertexArray(sf::Quads, rects.size() * 4);

  for (std::size_t i = 0; i < rects.size(); ++i) {
    const sf::Rect<float>& rect = rects[i];
    float left = 600.0f;
    float top = 400.0f;
    vertexArray[i * 4 + 0].position = sf::Vector2f(left, top);
    vertexArray[i * 4 + 1].position = sf::Vector2f(left + rect.width, top);
    vertexArray[i * 4 + 2].position = sf::Vector2f(left + rect.width, top + rect.height);
    vertexArray[i * 4 + 3].position = sf::Vector2f(left, top + rect.height);

    vertexArray[i * 4 + 0].color = color;
    vertexArray[i * 4 + 1].color = color;
    vertexArray[i * 4 + 2].color = color;
    vertexArray[i * 4 + 3].color = color;
  }

  return vertexArray;
}

sf::VertexArray createVertexArrayFromRectangleShapes(const std::vector<sf::RectangleShape>& shapes) {
  sf::VertexArray vertexArray(sf::Quads, shapes.size() * 4);

  for (std::size_t i = 0; i < shapes.size(); ++i) {
    const sf::RectangleShape& shape = shapes[i];
    const sf::Vector2f& position = {400, 400};
    const sf::Vector2f& size = shape.getSize();
    const sf::Color& color = sf::Color::Cyan;

    vertexArray[i * 4 + 0].position = position;
    vertexArray[i * 4 + 1].position = sf::Vector2f(position.x + size.x, position.y);
    vertexArray[i * 4 + 2].position = sf::Vector2f(position.x + size.x, position.y + size.y);
    vertexArray[i * 4 + 3].position = sf::Vector2f(position.x, position.y + size.y);

    vertexArray[i * 4 + 0].color = color;
    vertexArray[i * 4 + 1].color = color;
    vertexArray[i * 4 + 2].color = color;
    vertexArray[i * 4 + 3].color = color;
  }

  return vertexArray;
}

int main() {
  sf::RenderWindow window(sf::VideoMode(900, 800), "Drawing Speed Example", sf::Style::Close);

  /***
   * In this experiment we draw a large number of rectangles to the screen. They are all on top of
   * each other because the position is not relevant.
   */
  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);

  for (int i = 0; i < ShapeCount; i++) {
    sf::RectangleShape shape(sf::Vector2f(200, 200));
    shape.setPosition(0, 400);
    shape.setFillColor(sf::Color::Green);
    shapes.push_back(shape);
    rectangles.push_back(sf::Rect<float>(200, 400, 200, 200));
  }

  sf::Font font;
  /// This will generate an error message to stderr if it fails
  if (!font.loadFromFile("assets/fonts/consolas.ttf")) {
    exit(1);
  }

  sf::Text text("", font, 24);
  text.setFillColor(sf::Color::White);
  text.setPosition(10, 10);

  sf::Clock clock;
  sf::Clock timer;
  int frame_count = 0;
  int frame_interval = 20;

  sf::VertexArray v_shapes = createVertexArrayFromRectangleShapes(shapes);
  sf::VertexArray v_rectangles = createVertexArrayFromRects(rectangles);

  clock.restart();
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    std::ostringstream ss;
    ss << "Rendering " << ShapeCount << " objects to the screen:\n";
    uint32_t time;
    window.clear();

    window.draw(text);

    /// Draw a vector of sf::RectangleSahpe objects
    timer.restart();
    for (auto& shape : shapes) {
      window.draw(shape);
    }
    time = timer.restart().asMicroseconds();
    ss << "    shapes: " << std::setw(5) << time << "us \n";

    /// Create a shape from each rectangle and then draw it to the screen
    sf::RectangleShape r;
    for (auto& rect : rectangles) {
      r.setSize(rect.getSize());
      r.setFillColor(sf::Color::Blue);
      r.setPosition(200, 400);
      window.draw(r);
    }
    time = timer.restart().asMicroseconds();
    ss << "     rects: " << std::setw(5) << time << "us \n";

    sf::VertexArray v_rectangles = createVertexArrayFromRects(rectangles);
    window.draw(v_rectangles);
    time = timer.restart().asMicroseconds();
    ss << "   v_rects: " << std::setw(5) << time << "us ";
    window.draw(v_rectangles);
    time = timer.restart().asMicroseconds();
    ss << "  draw only = " << std::setw(5) << time << "us\n";

    sf::VertexArray v_shapes = createVertexArrayFromRectangleShapes(shapes);
    window.draw(v_shapes);
    time = timer.restart().asMicroseconds();
    ss << "  v_shapes: " << std::setw(5) << time << "us ";
    window.draw(v_shapes);
    time = timer.restart().asMicroseconds();
    ss << "  draw only = " << std::setw(5) << time << "us\n";

    time = clock.restart().asMicroseconds();
    ss << "frame time: " << std::setw(5) << time << "us \n";
    ss << "frame rate: " << std::setw(5) << (1000000 / time) << "fps \n";

    text.setString(ss.str());
    text.setPosition(10, 50);
    window.draw(text);
    window.display();
  }

  return 0;
}
