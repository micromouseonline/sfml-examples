#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <iostream>

/// https://www.sfml-dev.org/tutorials/2.6/graphics-vertex-array.php

int main() {
  /// Any antialiasing has to be set globally when creating the window:
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;  // the number of multisamplings to use. 4 is probably fine
  sf::RenderWindow window{sf::VideoMode(600, 400), "007 Vertex Arrays", sf::Style::Default, settings};
  window.setFramerateLimit(60);

  sf::Clock up_time{};

  /// you need the absolute or relative path to the font file.
  sf::Font aw_font;
  if (!aw_font.loadFromFile("./assets/fonts/consolas.ttf")) {
    exit(1);
  }
  /// A Vertex is a point with a colour and  a pair of texture coordinates.
  sf::Vertex vx1;
  vx1.position = sf::Vector2f(10.f, 50.f);     // set its position
  vx1.color = sf::Color::Red;                  // set its color
  vx1.texCoords = sf::Vector2f(100.f, 100.f);  // set its texture coordinates
  /// or
  sf::Vertex vx2(sf::Vector2f(10.f, 50.f), sf::Color::Red, sf::Vector2f(100.f, 100.f));

  /// primitives can be made from collections of vertices - a VertexArray
  /// This is actually a std::vector<sf::Vertex>
  /// create an array of 3 vertices that define a triangle primitive
  sf::VertexArray triangle(sf::Triangles, 3);

  // define the position of the triangle's points
  triangle[0].position = sf::Vector2f(10.f, 10.f);
  triangle[1].position = sf::Vector2f(110.f, 10.f);
  triangle[2].position = sf::Vector2f(60.f, 100.f);

  // define the color of the triangle's points
  triangle[0].color = sf::Color::Red;
  triangle[1].color = sf::Color::Blue;
  triangle[2].color = sf::Color::Green;

  /// We can also map a texture to the vertices
  /// This example is better done by other means. It just
  /// illustrates a point
  /// Load the texture
  sf::Texture texture;
  if (!texture.loadFromFile("./assets/images/tiles-64x64.png")) {
    std::cerr << "Unable to load texture\n";
    exit(1);
  }
  float w = 64;
  float h = 128;
  float ix = 10;
  float iy = 100;
  float tx = 0;
  float ty = 0;
  /// make the array of vertices
  /// The order is important. Each triangle shares the previous two vertices
  /// 0---1
  /// |   |
  /// 2---3

  sf::VertexArray floor_tile(sf::TriangleStrip, 4);
  floor_tile[0] = sf::Vertex(sf::Vector2f(ix, iy), sf::Color::White, sf::Vector2f(tx, ty));
  floor_tile[1] = sf::Vertex(sf::Vector2f(ix + 2 * w, iy), sf::Color::White, sf::Vector2f(tx + w, ty));
  floor_tile[2] = sf::Vertex(sf::Vector2f(ix, iy + 2 * h), sf::Color::White, sf::Vector2f(tx, ty + h));
  floor_tile[3] = sf::Vertex(sf::Vector2f(ix + 2 * w, iy + 2 * h), sf::Color::Yellow, sf::Vector2f(tx + w, ty + h));

  while (window.isOpen()) {
    /// process all the inputs
    sf::Event event{};
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed: {
          window.close();
          break;
        }
        case sf::Event::Resized: {
          sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
          window.setView(sf::View(visibleArea));  // or everything distorts
          break;
        }
        default:
          break;
      }
    }

    /// and redraw the window
    window.clear();

    /// sfml knows how to draw a vertex array
    window.draw(triangle);
    window.draw(floor_tile, &texture);

    window.display();
  }

  return 0;
}
