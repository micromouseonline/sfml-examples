//
// Created by peter on 19/11/2024.
//

#ifndef MAZE_H
#define MAZE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "drawing.h"

const int MAZE_WIDTH = (32);
const int NODES_PER_HORIZ_WALL_ROW = (MAZE_WIDTH);
const int NODES_PER_VERT_WALL_ROW = (MAZE_WIDTH - 1);
const int NUMBER_OF_HORIZ_WALLS = (NODES_PER_HORIZ_WALL_ROW);
const int NUMBER_OF_VERT_WALLS = (NODES_PER_VERT_WALL_ROW + 2);
const int NUMBER_OF_HORIZ_WALL_ROWS = (MAZE_WIDTH + 1);
const int NUMBER_OF_VERT_WALL_ROWS = (MAZE_WIDTH);
const int NUMBER_OF_WALLS = ((NUMBER_OF_HORIZ_WALLS * NUMBER_OF_HORIZ_WALL_ROWS) + (NUMBER_OF_VERT_WALLS * NUMBER_OF_VERT_WALL_ROWS));

const int LOCATION_DELTA_NORTH = (MAZE_WIDTH * 2 + 1);
const int LOCATION_DELTA_NORTH_EAST = (MAZE_WIDTH + 1);
const int LOCATION_DELTA_EAST = (1);
const int LOCATION_DELTA_SOUTH_EAST = (-MAZE_WIDTH);
const int LOCATION_DELTA_SOUTH = (-(MAZE_WIDTH * 2 + 1));
const int LOCATION_DELTA_SOUTH_WEST = (-(MAZE_WIDTH + 1));
const int LOCATION_DELTA_WEST = (-1);
const int LOCATION_DELTA_NORTH_WEST = (MAZE_WIDTH);

const float WALL_THICKNESS = (12.0f * 16.0f / MAZE_WIDTH);
const float WALL_LENGTH = (166.0f * 16.0f / MAZE_WIDTH);
const float CELL_SIZE = ((180.0 * 16.0f) / MAZE_WIDTH);

enum class WallState {
  KnownAbsent,   //
  KnownPresent,  //
  Unknown,       //
  Virtual        //
};

struct Wall {
  WallState state = WallState::Unknown;
  sf::RectangleShape shape;
};

enum class Direction { North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest };
/***
 * The MazeManager class looks after the maze for the application, not for the robot or its controller.
 * How they do that is their concern.
 *
 * However, the method used for storing each wall once is the basis of Harjit's diagonal solver.
 * TODO: Create a README about hte method and the arrangement of walls
 *
 * MazeManager can load, save, print and draw a maze to te screen. It stores the actual real-world maze
 * that the simulated robot explored. Each wall has one of the states shown above and a sf::RectangleShape
 * describing its position in the window. This assumes one pixel per mm. The view can be scaled to fit
 * the entire maze in a more manageable screen size.
 *
 * A structure called Wall is used to store the state and shape of each wall.
 *
 * The rectangles are pre-computed for an entire 32x32 sized maze when the manager is instantiated.
 * All the walls states are set to unknown.
 *
 * Each Wall can be located uniquely by the method GetWallIndex(x,y,Direction).
 *
 * The MazeManager also creates and stores all the posts in the maze using the same Wall struct
 * except that these are all set to KnownPresent.
 *
 * The reason for storing the posts is so that the sensor emulation and collision detection can
 * work on a more limited set obstacles around the current mouse location rather than test
 * against all 4000+ wall and post location in a 32x32 maze.
 *
 */

class MazeManager {
 public:
  /// these constants are mostly needed for testing

  MazeManager() {
    // TODO: ADD the posts
    InitMaze();  //
  };

  ~MazeManager() = default;

  void LoadMazeFromFile(const std::string& filename) {
    (void)filename;
    //
  }

  void LoadMazeFromString(const std::string& mazeData) {
    (void)mazeData;
    //
  }

  bool IsHorizontal(int wall_index) {
    return ((wall_index % LOCATION_DELTA_NORTH) < NUMBER_OF_HORIZ_WALLS);  //
  }

  int GetWallIndex(int cellX, int cellY, Direction direction) const {
    int index = cellX + LOCATION_DELTA_NORTH * cellY;
    switch (direction) {
      case Direction::North:
        index += LOCATION_DELTA_NORTH;
        break;
      case Direction::East:
        index += LOCATION_DELTA_NORTH_EAST;
        break;
      case Direction::South:
        index += 0;
        break;
      case Direction::West:
        index += LOCATION_DELTA_NORTH_WEST;
        break;
      default:
        index = 0;  // even though this is wrong.
        break;
    }
    return index;
  }

  sf::Color GetWallColor(WallState state) {
    switch (state) {
      case WallState::KnownAbsent:
        return m_knownAbsentColor;
      case WallState::KnownPresent:
        return m_knownPresentColor;
      case WallState::Unknown:
        return m_unknownColor;
      case WallState::Virtual:
        return m_virtualColor;
      default:
        return m_unknownColor;
    }
  }

  void SetWallState(int index, WallState state) {
    m_walls[index].state = state;
    m_walls[index].shape.setFillColor(GetWallColor(state));
  }

  void SetWallState(int x, int y, Direction direction, WallState state) {
    int index = GetWallIndex(x, y, direction);
    SetWallState(index, state);
  }

  void InitWall(int x, int y, Direction direction, WallState state) {
    sf::RectangleShape wall_shape;
    /// get the top left
    float cx = float(x) * CELL_SIZE + 25.0f;
    float cy = (CELL_SIZE * MAZE_WIDTH) - (float)(y + 1) * CELL_SIZE + 25.0f;
    switch (direction) {
      case Direction::North:
        cx += WALL_THICKNESS + 1;
        wall_shape.setSize({WALL_LENGTH, WALL_THICKNESS});
        break;
      case Direction::East:
        cx += CELL_SIZE;
        cy += WALL_THICKNESS + 1;
        wall_shape.setSize({WALL_THICKNESS, WALL_LENGTH});
        break;
      case Direction::South:
        cx += WALL_THICKNESS + 1;
        cy += CELL_SIZE;
        wall_shape.setSize({WALL_LENGTH, WALL_THICKNESS});
        break;
      case Direction::West:
        cy += WALL_THICKNESS + 1;
        wall_shape.setSize({WALL_THICKNESS, WALL_LENGTH});
        break;
      default:
        break;  // this is actually an error.
    }
    //    wall_shape.setSize({WALL_THICKNESS, WALL_THICKNESS});
    int index = GetWallIndex(x, y, direction);
    wall_shape.setPosition(cx, cy);
    //    wall_shape.setSize({CELL_SIZE / 2, CELL_SIZE / 2});
    m_walls[index].shape = wall_shape;
    SetWallState(index, state);
  }

  void InitMaze() {
    for (int y = 0; y < MAZE_WIDTH; y++) {
      for (int x = 0; x < MAZE_WIDTH; x++) {
        InitWall(x, y, Direction::North, WallState::Unknown);
        InitWall(x, y, Direction::West, WallState::Unknown);
      }
    }
    for (int i = 0; i < MAZE_WIDTH; i++) {
      InitWall(i, 0, Direction::South, WallState::KnownPresent);
      InitWall(i, MAZE_WIDTH - 1, Direction::North, WallState::KnownPresent);
      InitWall(0, i, Direction::West, WallState::KnownPresent);
      InitWall(MAZE_WIDTH - 1, i, Direction::East, WallState::KnownPresent);
    }
    InitWall(0, 0, Direction::East, WallState::KnownPresent);
    InitWall(0, 0, Direction::North, WallState::KnownAbsent);

    std::cout << "Maze Width " << MAZE_WIDTH << std::endl;
    std::cout << "Wall Length " << WALL_LENGTH << std::endl;
    std::cout << "Wall Width " << WALL_THICKNESS << std::endl;
    sf::Rect rect(0.0f, 0.0f, 0.0f, 0.0f);
    rect = m_walls[0].shape.getGlobalBounds();
    std::cout << "Rect: " << rect.left << " " << rect.top << " " << rect.width << " " << rect.height << std::endl;
    rect = m_walls[1].shape.getGlobalBounds();
    std::cout << "Rect: " << rect.left << " " << rect.top << " " << rect.width << " " << rect.height << std::endl;
  }

  void Render(sf::RenderWindow& window) {
    sf::RectangleShape base({2880.0f, 2880.0f});
    base.setPosition(25.0f, 25.0f);
    base.setFillColor(sf::Color(30, 30, 30, 64));
    window.draw(base);

    for (int i = 0; i < NUMBER_OF_WALLS; i++) {
      window.draw(m_walls[i].shape);
    }
    draw_line(window, {25.0, 25.0}, {2880 + 25, 2880 + 25}, sf::Color::Cyan);
  }

 private:
  Wall m_walls[NUMBER_OF_WALLS];  // Array of wall states
  sf::Color m_knownPresentColor = (sf::Color::Red);
  sf::Color m_knownAbsentColor = (sf::Color::Black);
  sf::Color m_unknownColor = (sf::Color(128, 128, 128, 64));
  sf::Color m_virtualColor = (sf::Color::Blue);
};

#endif  // MAZE_H
