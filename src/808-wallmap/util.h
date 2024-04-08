//
// Created by peter on 06/04/24.
//

#pragma once

enum Direction { DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW, DIR_COUNT };
const int wall_offset[] = {65, 33, 1, -32, -65, -33, -1, 32};

enum WallState {
  EXIT,
  WALL,
  UNKNOWN,
  VIRTUAL,
};
/** directions are
 *      7 0 1
 *       \|/
 *     6 -+- 2
 *       /|\
 *      5 4 3
 *
 * @param x 0..31
 * @param y 0..31
 * @param dir 0..7
 * @return
 */
inline int wall_id(int x, int y, int dir) {
  int i = 65 * x + y;
  switch (dir) {
    case DIR_N:
      return i + 65;
    case DIR_E:
      return i + 33;
    case DIR_W:
      return i + 32;
    default:
      return i;
  };
}

inline bool is_vertical(int wall_id) {
  return (wall_id % 65) >= 32;
}