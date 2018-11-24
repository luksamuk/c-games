/*
 * game-of-life.c
 * Simple implementation of Conway's Game of Life for console.
 *
 * This project is distributed under the BSD 2-Clause License.
 *
 * Copyright (c) 2018, Lucas Vieira
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdio.h>
#include <string.h>

#define BOARD_WIDTH  20
#define BOARD_HEIGHT 10

// The board is looked into as if it were a SIDExSIDE board, but it is actually
// an infinite playground. Imagine the surface of a donut.
#define BOARD_SIZE   (BOARD_WIDTH * BOARD_HEIGHT)

// Translates (X, Y) coordinate into the index of an 1D array.
#define coord_to_idx(x, y) ((y * BOARD_WIDTH) + x)

// Cosmetic appearance of alive and dead cells.
#define printable_state(state) ((state == CELL_ALIVE) ? 'o' : ' ')

// Cute macro to ensure that increasing coordinates beyond limits actually
// wrap around playground, e.g. a negative X coordinate should only mean that
// you're going left and reappearing on the right side.
#define wrap_coords(x, y)	      				  	 \
    x = (x < 0) ? (BOARD_WIDTH  - 1) : ((x > BOARD_WIDTH  - 1) ? 0 : x); \
    y = (y < 0) ? (BOARD_HEIGHT - 1) : ((y > BOARD_HEIGHT - 1) ? 0 : y);


// Cell state
typedef enum {
    CELL_DEAD,
    CELL_ALIVE
} cell_t;

// Neighbor directions, including diagonal ones
typedef enum {
    DIR_NORTH,
    DIR_SOUTH,
    DIR_WEST,
    DIR_EAST,
    DIR_NORTHWEST,
    DIR_NORTHEAST,
    DIR_SOUTHWEST,
    DIR_SOUTHEAST
} dir_t;


// Checks whether a neighbor of cell (x, y) in given direction
// is dead or alive. Takes the coordinate wrapping idea into consideration.
cell_t
check_neighbor(const cell_t* board, int x, int y, dir_t direction)
{
    switch(direction) {
    case DIR_NORTH:     y--;      break;
    case DIR_SOUTH:     y++;      break;
    case DIR_WEST:      x--;      break;
    case DIR_EAST:      x++;      break;
    case DIR_NORTHWEST: x--; y--; break;
    case DIR_NORTHEAST: x++; y--; break;
    case DIR_SOUTHWEST: x--; y++; break;
    case DIR_SOUTHEAST: x++; y++; break;
    default: break; // wat
    }

    // Check for overflow. This is where the donut shape comes into play.
    // If we only wanted a fixed number of places, we could just return
    // CELL_DEAD on these cases.
    wrap_coords(x, y);

    return board[coord_to_idx(x, y)];
}

// Returns the number of living neighbors around cell at (x, y).
unsigned
get_living_neighbors(const cell_t* board, int x, int y)
{
    unsigned n_elements = 0;
    
    // CLEVER TRICK: Enums are just cute names for underlying
    // `int`s. Therefore, we can just loop from 0 to <8 (as there are
    // 8 possible directions) to get our number of neighbors.
    size_t i;
    for(i = 0; i < 8; i++)
	if(check_neighbor(board, x, y, (dir_t)i) == CELL_ALIVE)
	    n_elements++;

    return n_elements;
}

// Applies life and death rules onto cell (x, y). Notice that while the
// new state is recorded, the rules are applied based on the state of the
// board prior to current iteration.
void
apply_rules(cell_t* current, const cell_t* old, int x, int y)
{
    unsigned neighbors = get_living_neighbors(old, x, y);
    size_t index = coord_to_idx(x, y);
   
    if(current[index] == CELL_ALIVE) {
	// RULE 1: Any live cell with fewer than two live neighbors dies,
	// as if by underpopulation.
	// RULE 2: Any live cell with two or three live neighbors lives on
	// to the next generation.
	// RULE 3: Any live cell with more than three live neighbors dies,
	// as if by overpopulation.
	if((neighbors < 2) || (neighbors > 3))
	   current[index] = CELL_DEAD;
    }
    // RULE 4: Any dead cell with exactly three live neighbors becomes a
    // live cell, as if by reproduction.
    else if(neighbors == 3) current[index] = CELL_ALIVE;
}

// Generic function for printing the board.
void
print_board(cell_t* board) {
    size_t x, y;
    for(y = 0; y < BOARD_HEIGHT; y++) {
	putchar('|');
	for(x = 0; x < BOARD_WIDTH; x++) {
	    size_t index = coord_to_idx(x, y);
	    printf("%c", printable_state(board[index]));
	    if(y != BOARD_WIDTH - 1) putchar(' ');
	}
	puts("|\n");
    }
}


// Inserts a glider at position. Notice that the glider's shape is drawn
// starting at its top left corner.
void
add_glider(cell_t* board, int x, int y)
{
    int positions[] =
	{
	 1, 0,
	 2, 1,
	 0, 2,
	 1, 2,
	 2, 2
	};
    size_t i;
    for(i = 0; i < 5; i++) {
	int xp = x + positions[(i * 2)],
	    yp = y + positions[(i * 2) + 1];
	wrap_coords(xp, yp);
	board[coord_to_idx(xp, yp)] = CELL_ALIVE;
    }
}




int
main(void)
{
    cell_t old_board[BOARD_SIZE];
    cell_t cur_board[BOARD_SIZE];

    int iterate = 1;
    unsigned long long generation = 0;

    // Initialize boards
    memset(cur_board, 0, BOARD_SIZE * sizeof(cell_t));
    memset(old_board, 0, BOARD_SIZE * sizeof(cell_t));

    // The board is defined as WIDTH * HEIGHT due to caching
    // improvements, but it does not affect how the program works.

    // Derp
    add_glider(cur_board, 2, 2);

    while(iterate) {
	// Print board
	print_board(cur_board);
	printf("Generation: %llu\n", generation++);
	
	// Copy old state
	memcpy(old_board, cur_board, BOARD_SIZE * sizeof(cell_t));

	// Iterate over and apply rules
	int x, y;
	for(y = 0; y < BOARD_HEIGHT; y++) {
	    for(x = 0; x < BOARD_WIDTH; x++)
		apply_rules(cur_board, old_board, x, y);
	}

	// Await keypress, and quit if `q` is rpessed
	if(getchar() == 'q')
	    iterate = 0;
    }
    
    return 0;
}
