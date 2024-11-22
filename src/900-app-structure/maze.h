//
// Created by peter on 19/11/2024.
//

#ifndef MAZE_H
#define MAZE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

enum Direction { NORTH, EAST, SOUTH, WEST };

struct Maze {
  /***
   * this is a mock class that represents a maze.
   * It just maintains a list of walls and draws them to
   * the main window to provide obstacles for the robot.
   */
  Maze() {}

  void add_wall(int x, int y, int direction) {
    /// we don't care that it is all upside down
    auto wall = sf::RectangleShape(sf::Vector2f(166.f, 12.f));
    wall.setFillColor(sf::Color(88, 0, 0, 255));
    sf::Vector2f cell_top_left = {static_cast<float>(x) * 180.0f, static_cast<float>(y) * 180.0f};
    switch (direction) {
      case NORTH:
        wall.setPosition(cell_top_left + sf::Vector2f(13.f, 0.f));  //
        break;
      case SOUTH:
        wall.setPosition(cell_top_left + sf::Vector2f(13.f, 180.f));
        break;
      case WEST:
        wall.setSize(sf::Vector2f(sf::Vector2f(12.f, 166.f)));
        wall.setPosition(cell_top_left + sf::Vector2f(0.f, 13.f));
        break;
      case EAST:
        wall.setSize(sf::Vector2f(sf::Vector2f(12.f, 166.f)));
        wall.setPosition(cell_top_left + sf::Vector2f(180.f, 13.f));
        break;
      default:
        break;
    }
    walls.push_back(wall);
  };
  /// create a wall and add it to the walls list
  void add_posts(int w, int h) {
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        sf::RectangleShape post(sf::Vector2f(12, 12));
        post.setFillColor(sf::Color(88, 0, 0, 255));
        post.setPosition(x * 180, y * 180);
        walls.push_back(post);
      }
    }
  }

  void draw(sf::RenderTarget& target) {
    for (auto& wall : walls) {
      target.draw(wall);
    }
  }
  // create vector of pointers to walls
  std::vector<sf::RectangleShape> walls;
};

#endif  // MAZE_H
