//
// Created by peter on 22/04/24.
//

#pragma once

//// Set up all the data for a 32x32 maze
//// but allow a 16x16 section to be used if needed
/***
 * Walls in the maze are given an individual id number. This is slightly
 * awkward because the number of horizontal and vertical walls in a row are
 * different. Storing only the North and East walls would make for some
 * improvements in storage but that might ve outweighed by a computational
 * cost when dealing with the special cases for the West and South boundaries
 * of the maze
 */
#define MAZE_WIDTH (32)
#define NUMBER_OF_HORIZ_WALLS (MAZE_WIDTH)
#define NUMBER_OF_HORIZ_WALL_ROWS (MAZE_WIDTH + 1)
#define NUMBER_OF_VERT_WALLS (MAZE_WIDTH + 1)
#define NUMBER_OF_VERT_WALL_ROWS (MAZE_WIDTH)
#define WALLS_PER_ROW (2 * NUMBER_OF_HORIZ_WALLS + 1)
#define DELTA_NORTH (WALLS_PER_ROW)

// clang-format off

#define WALL_COUNT ( (MAZE_WIDTH * (MAZE_WIDTH+1)) + (MAZE_WIDTH * (MAZE_WIDTH+1)))
static_assert(WALL_COUNT == 2112, "Messed up the number of locations");

/// The offset of one wall from another
constexpr int location_delta[] = {
    MAZE_WIDTH * 2 + 1,
    MAZE_WIDTH + 1,
    1,
    -MAZE_WIDTH,
    -(MAZE_WIDTH * 2 + 1),
    -(MAZE_WIDTH + 1),
    -1,
    +MAZE_WIDTH,
};

/** directions are
 *      7 0 1
 *       \|/
 *     6 -+- 2
 *       /|\
 *      5 4 3
 */

enum Direction {
  DIR_N = 0,      //
  DIR_NE = 1,     //
  DIR_E = 2,      //
  DIR_SE = 3,     //
  DIR_S = 4,      //
  DIR_SW = 5,     //
  DIR_W = 6,      //
  DIR_NW = 7,     //
  DIR_NONE = 8,   // used for finding the node at the cell centre
  DIR_COUNT = 8,  //
  DIR_BLOCKED     //
};

enum FloodError {
    E_FLOOD_THROUGH_POST = -1,
    E_FLOOD_STARTS_AT_WALL = -2,
    E_FLOOD_BAD_NEIGHBOUR = -3,
};

// clang-format on
