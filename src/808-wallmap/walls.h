//
// Created by peter on 22/04/24.
//

#pragma once

#include "maze_constants.h"
// free functions for manipulating wall data

enum WallState {
  EXIT,
  WALL,
  UNKNOWN,
  VIRTUAL,
};

/***
 * Identify the id number of an individual wall from
 * the cell location and direction
 * @param   x 0..MAZE_WIDTH-1
 * @param   y 0..MAZE_WIDTH-1
 * @param dir 0..DIR_COUNT-1
 * @return
 */

inline int wall_id(int x, int y, int dir) {
  int i = WALLS_PER_ROW * x + y;
  switch (dir) {
    case DIR_N:
      return i + WALLS_PER_ROW;
    case DIR_E:
      return i + MAZE_WIDTH + 1;
    case DIR_W:
      return i + MAZE_WIDTH;
    case DIR_S:
      return i;
    default:
      return DIR_BLOCKED;
  };
}

/***
 * Identify horizontal walls.
 * Each row of cells holds (width*2+1) walls. Of those, the first
 * (width) walls are the south walls of a cell and so must be
 * horizontal. The rest are the West walls of a cell
 *
 * @param wall_id
 * @return : true if horizontal, false if vertical
 */
inline bool is_horizontal(int wall_id) {
  if ((wall_id % DELTA_NORTH) < MAZE_WIDTH) {
    return true;
  } else {
    return false;
  }
}

inline bool is_vertical(int wall_id) {
  return !is_horizontal(wall_id);
}
inline int get_cell_x(int wall_id) {
  return ((wall_id % WALLS_PER_ROW)) % MAZE_WIDTH;
}

inline int get_cell_y(int wall_id) {
  return wall_id / WALLS_PER_ROW;
}

inline int start_gate() {
  return wall_id(0, 0, DIR_N);
}

inline int neighbour(int index, Direction dir) {
  if (dir < DIR_COUNT) {
    return index + location_delta[dir];
  } else {
    return E_FLOOD_BAD_NEIGHBOUR;  // TODO: this will break the caller. Handle the error better
  }
}

inline Direction direction_between(int &src_wall, int &dst_wall) {
  int delta = dst_wall - src_wall;
  Direction direction = DIR_NONE;
  // TODO: detect illegal moves through posts
  switch (delta) {
    case 0:
      direction = DIR_NONE;
      break;
    case location_delta[DIR_N]:
      direction = DIR_N;
      break;
    case location_delta[DIR_NE]:
      direction = DIR_NE;
      break;
    case location_delta[DIR_E]:
      direction = DIR_E;
      break;
    case location_delta[DIR_SE]:
      direction = DIR_SE;
      break;
    case location_delta[DIR_S]:
      direction = DIR_S;
      break;
    case location_delta[DIR_SW]:
      direction = DIR_SW;
      break;
    case location_delta[DIR_W]:
      direction = DIR_W;
      break;
    case location_delta[DIR_NW]:
      direction = DIR_NW;
      break;
    default:
      direction = DIR_BLOCKED;
  }
  return direction;
}

inline void center_number(char *str, int width, uint16_t num) {
  int len = snprintf(NULL, 0, "%d", num);
  int pad = (width - len) / 2;
  sprintf(str, "%*s%d%*s", pad, "", num, width - pad - len, "");
}

////////////////////////////////////////////////////////////////////////////////////
/***
 * This is te set of information we want to store about a wall when flooding the
 * maze. It is important to note that instances of WallData do not know their own
 * ID.
 *
 * Bitfields:
 * If you want to make this as small as possible consider using a packed structure
 * for the data
 *
 * bool m_is_exit : 1;
 * bool m_is_queued : 1
 */

class WallData {
 public:
  WallData() { reset(); };
  //  WallData(int x, int y, Direction wall_direction) { reset(); }

  void reset() {
    m_state = UNKNOWN;
    m_is_active = false;
    m_is_queued = false;
    m_was_queued = false;
    m_direction = DIR_NONE;
    m_is_on_path = false;
    m_predecessor = 0;  // the wall we came from to get here
    m_cost = UINT16_MAX;
  }

  bool is_active() { return m_is_active; }

  void set_active(bool state) { m_is_active = state; }

  bool is_queued() const { return m_is_queued; }

  void set_queued(bool state) { m_is_queued = state; }

  bool is_exit() const { return m_state == EXIT; }

  void set_state(WallState state) { m_state = state; }

  void set_on_path(bool state) { m_is_on_path = state; }

  bool is_on_path() { return m_is_on_path; }

  Direction direction() const { return m_direction; }

  void set_direction(Direction dir) { m_direction = dir; }

  uint16_t cost() { return m_cost; }

  void set_cost(uint16_t cost) { m_cost = cost; }

  int predecessor() { return m_predecessor; }

  void set_predecessor(int index) { m_predecessor = index; }

 private:
  bool m_is_queued = false;
  bool m_was_queued = false;
  bool m_is_active = false;
  bool m_is_on_path = false;
  uint16_t m_predecessor = 0;        // the wall we came from to get here
  Direction m_direction = DIR_NONE;  // The direction we are passing through this wall
  uint16_t m_cost = UINT16_MAX;
  WallState m_state = UNKNOWN;
};
