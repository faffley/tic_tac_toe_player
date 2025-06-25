//Tic tac toe game, with an algorithmic opponent

#include<stdio.h>
#include<stdlib.h>

enum {
    GRID_X_DIM = 3,
    GRID_Y_DIM = 3,
    GRID_TOTAL = 9,
    LINE_MAX = 256,
    GRID_MAP_HASH_MAX = 499,
};

typedef enum Tile Tile;

enum Tile {
    EMPTY = 0,
    X_PL = 1,
    O_PL = 2,
    TOTAL_TILES = 3, //Error condition return as well. 
};

//this is just a switch case
//returns the null character on error. 

char const tile_to_char(Tile const t) {
    switch (t) {
    case EMPTY: 
        return 'E';
    case X_PL:
        return 'X';
    case O_PL:
        return 'O';
    default:
        return '\0';
    }
}
//We use tiles to keep track of the current player as well. 
//X is the starting player by default
//Player uses EMPTY and TOTAL_TILES as error states

typedef Tile Player;
char const * const player_to_string(Player const p) {
    switch (p) {
    case X_PL:
        return "Player X";
    case O_PL:
        return "Player O";
    default:
        return "Error, no current player";
    }
}

Player next_player(Player p) {
    switch (p) {
    case X_PL:
        return O_PL;
    case O_PL:
        return X_PL;
    default:
        return EMPTY;
    }
}

//This is just a wrapper around a grid.

//We access using x, y coordinates, 0 <= x, y <= 3.

//Contains the world state

typedef struct Grid Grid;
//Has grid data and turn
struct Grid {
    Tile data [GRID_TOTAL];
    Player player;
};

Grid* reset(Grid* g) {
    for (size_t i = 0; i < GRID_TOTAL; i++) {
        g->data[i] = EMPTY;
    }
    g->player = X_PL;
    return g;
}

Grid* init_new_grid() {
    Grid *p = malloc(sizeof(Grid));
    reset(p);
    return p;
}

size_t get_index (size_t x, size_t y) {
    return y*GRID_X_DIM + x;
}

//Returns TOTAL if out of bounds error
Tile get(Grid const* g, size_t x, size_t y) {
    if (x < 3 && y < 3) {
        return g->data[get_index(x, y)];
    }
    else {
        return TOTAL_TILES;
    }
}

//Tiles are copyable, do not need pointers
//Returns TOTAL if out of bounds
Tile set(Grid* g, size_t x, size_t y, Tile t) {
    if (x < 3 && y < 3) {
        g->data[get_index(x, y)] = t;
        return t;
    }
    else {
        return TOTAL_TILES;
    }
}

//RETURNS TOTAL_TILES on failure, ie if the spot at x, y was already taken.
Tile move(Grid* g, size_t x, size_t y) {
    Tile ret = TOTAL_TILES;
    if (get(g, x, y) == EMPTY) {
        ret = set(g, x, y, g->player);
        g->player = next_player(g->player);
    }
    return ret;
}

//Doesn't check errors, prints empty character on error. 
void print_grid(Grid const * const g) {
    char current = 'E';
    printf("Current turn: %s\n", player_to_string(g->player));
    for (size_t y = 0; y < GRID_Y_DIM; y++) {
        for (size_t x = 0; x < GRID_X_DIM; x++) {
            current = tile_to_char(g->data[get_index(x,y)]);
            printf("%c ", current);
            //May print '\0' which is empty?
            //THIS MIGHT BE A BUG, NOT QUITE SURE
        }
        printf("\n");
    }
    printf("\n");
}

//Copies without allocation
Grid* copy_grid_into(Grid const* const p1, Grid* p2) {
    if (p2) {
        for (size_t i = 0; i < GRID_TOTAL; i++) {
            p2->data[i] = p1->data[i];
        }
        p2->player = p1->player;
    }
    return p2;
}
//Copies with allocation
//Returns null on failure. 
Grid* copy(Grid const * const p) {
    return copy_grid_into(p, malloc(sizeof(Grid)));
}

void destroy(Grid* g) {
    free(g);
}

bool equals(Grid const * const g1, Grid const * const g2) {
    if (g1 && g2) {
        if (g1->player != g2->player) {
            return false;
        }
        for (size_t i = 0; i < GRID_TOTAL; i++) {
            if (g1->data[i] != g2->data[i]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}


//Checks if the last move played at x, y is a winning move. 
bool is_winning_move(Grid const * const g, size_t x, size_t y) {
    Player maybe_winner = get(g, x, y);
    //First checks if the move was possible
    if (maybe_winner == EMPTY) {
        return false;
    }

    //Check rook moves
    //Horizontal check
    bool horizontal_win = true;
    for (size_t i = 0; i < GRID_X_DIM; i++) {
        if (get(g, i, y) != maybe_winner) {

            horizontal_win = false;
            break;
        }
    }

    if (horizontal_win) {
        return true;
    }

    //Vertical check
    bool vertical_win = true;
    for (size_t i = 0; i < GRID_Y_DIM; i++) {
        if (get(g, x, i) != maybe_winner) {
            vertical_win = false;
            break;
        }
    }

    if (vertical_win) {
        return true;
    }

    //Check diagonals
    //Positive slope diagonal
    if (x - y == 0) {
        bool diag_win = true;
        for (size_t i = 0; i < GRID_Y_DIM; i++) {
            if (get(g, i, i) != maybe_winner) {
                diag_win = false;
                break;
            }
        }
        if (diag_win) {
            return true;
        }
    }
    //Check other diagonal
    if (x + y == 2) {
        bool diag_win = true;
        for (size_t i = 0; i < GRID_Y_DIM; i++) {
            if (get(g, i, 2 - i) != maybe_winner) {
                diag_win = false;
                break;
            }
        }
        if (diag_win) {
            return true;
        }
    }
    return false;
}


Player has_won(Grid const * const g) {
    //Checks with 1, 1
    Player pot_winner = get(g, 1, 1);
    if (pot_winner != EMPTY) {
        //Diagonal checks
        if ( ( pot_winner == get(g, 0, 0) ) && ( pot_winner == get(g, 2, 2) ) ) {
            return pot_winner;
        } else if ( (pot_winner == get(g, 2, 0)) && (pot_winner == get(g, 0, 2)) ) {
            return pot_winner;
        } else if ((pot_winner == get(g, 1, 0)) && (pot_winner == get(g, 1, 2))) {
            return pot_winner;
        } else if ( (pot_winner) == get(g, 0, 1) && (pot_winner == get(g, 2, 1))) {
            return pot_winner;
        }
    }

    //Check the boundary wins

    pot_winner = get(g, 0, 0);

    if (pot_winner != EMPTY) {
        if (( pot_winner == get(g, 1, 0) ) && ( pot_winner == get(g, 2, 0) ) ) {
            return pot_winner;
        } else if (( pot_winner == get(g, 0, 1) ) && ( pot_winner == get(g, 0, 2) ) ) {
            return pot_winner;
        }
    }

    pot_winner = get(g, 2, 2);

    if (pot_winner != EMPTY) {
        if (( pot_winner == get(g, 1, 2) ) && ( pot_winner == get(g, 0, 2) ) ) {
            return pot_winner;
        } else if (( pot_winner == get(g, 2, 1) ) && ( pot_winner == get(g, 2, 0) ) ) {
            return pot_winner;
        }
    }
    //No winner
    return EMPTY;
}

//Returns empty if no one has won yet.
/*
Player has_won(Grid const * const g) {
    //Checks including 1, 1
    
    if (is_winning_move(g, 1, 1)) {
        return get(g, 1, 1);
    } else {
        //Check vert and horizontal at 0,0 and 2,2
        
        Player pot_winner = get(g, 0, 0);

        if (pot_winner != EMPTY) {
            bool horizontal_win = true;
            for (size_t i = 0; i < GRID_X_DIM; i++) {
                if (get(g, i, 0) != pot_winner) {
                    horizontal_win = false;
                    break;
                }
            }
            if (horizontal_win) {
                return pot_winner;
            }
            bool vertical_win = true;
            for (size_t j = 0; j < GRID_Y_DIM; j++) {
                if (get(g, 0, j) != pot_winner) {
                    vertical_win = false;
                    break;
                }
            } 
            if (vertical_win) {
                return pot_winner;
            }
        } else {
            pot_winner = get(g, 2, 2);
            bool horizontal_win = true;
            for (size_t i = 0; i < GRID_X_DIM; i++) {
                if (get(g, i, 2) != pot_winner) {
                    horizontal_win = false;
                    break;
                }
            }
            if (horizontal_win) {
                return pot_winner;
            }
            bool vertical_win = true;
            for (size_t j = 0; j < GRID_Y_DIM; j++) {
                if (get(g, 2, j) != pot_winner) {
                    vertical_win = false;
                    break;
                }
            } 
            if (vertical_win) {
                return pot_winner;
            }
        }
        return EMPTY;
    }
}
*/
bool is_full(Grid const * const g) {
    for (size_t i = 0; i < GRID_TOTAL; i++) {
        if (g->data[i] == EMPTY) {
            return false;
        }
    }
    return true;
}

typedef struct GridList GridList;

struct GridList {
    Grid const * grid;
    GridList* next;
};

//Empty tiles number should always match the size of the array possible moves. 

//Creates new empty board
GridList* init_GridList_from_pointer(GridList* pt) {
    if (pt) {
        pt->grid = init_new_grid();
        pt->next = nullptr;
    }
    return pt;
}
//Grid is borrowed into GridList
GridList* init_from_grid (GridList* pt, Grid const * const grid) {
    if (pt) {
        pt->next = nullptr;
        pt->grid = grid;
        return pt;
    }
    return nullptr;

}

GridList* init_from_grid_copy(GridList* pt, Grid const * const g) {
    if (pt && (pt->grid = copy(g))) {
        pt->next = nullptr;
        return pt;
    }
    //Allocation error
    return nullptr;
}

GridList* new_grid_copy(Grid const * const g) {
    return init_from_grid_copy(malloc(sizeof(GridList)), g);
}

//Only use from new!
//Commented out: GridList doesn't own the grid!!

void destroy_grid_list_keep_grids(GridList* p) {
    if (p) {
        GridList* iter = p;
        while(iter != nullptr) {
            GridList* next = iter->next;
            free(iter);
            iter = next;
        }
    }
}


/*
typedef struct GameTree GameTree;
struct GameTree {
    Grid const* current_grid;
    GridList* possible_moves; //List of next values
    size_t empty_tiles; //is the same as the size of possible moves.
    Player player;
};

//This consumes the pointer, do not use current_grid afterwards
GameTree* init(Grid const* current_grid, Player player) {
    GameTree *ret = malloc(sizeof(GameTree));

    ret->current_grid = current_grid;
    ret->player = player;
    ret->possible_moves = find_possible_moves(current_grid, player);
    return ret;
}
*/
//Keeps track of state 

//Has the same ordering as Tile and Player
//UNKNOWN = EMPTY 
//X_WIN = X_PL
//O_WIN = O_PL
//DRAW == TOTAL
enum WinState {
    UNKNOWN = 0, // Sentinel value
    X_WIN = 1,
    O_WIN = 2,
    DRAW = 4,
};

typedef enum WinState WinState;

char const * const state_to_string(WinState state) {
    switch (state) {
    case X_WIN:
        return "X wins";
    case O_WIN:
        return "O wins";
    case DRAW:
        return "Draw";
    default:
        return "Win state unknown";
    }
} 

//Used to make a hashmap

typedef struct GridStateNode GridStateNode;
struct GridStateNode {
    Grid const * grid;
    WinState state;
    GridStateNode* next;
};

//Memoization solution

//The map will own all the Grid objects that we make.
struct GridStateMap {
    GridStateNode* data [GRID_MAP_HASH_MAX];
};

typedef struct GridStateMap GridStateMap;

GridStateMap* init_map(GridStateMap* mpt) {
    if (mpt) {
        for (size_t i = 0; i < GRID_MAP_HASH_MAX; i++) {
            mpt->data[i] = nullptr;
        }
    }
    return mpt;
}


GridStateMap* new_map() {
    return init_map(malloc(sizeof(GridStateMap)));
}

//Does not consume
size_t hash_grid(Grid const g) {
    size_t h = 0;
    h += g.player;
    for (size_t i = 0; i < GRID_TOTAL; i++) {
        h += g.data[i];
    }
    return h % GRID_MAP_HASH_MAX;
}
//CANNOT DOUBLE INSERT!!
//If insert is true, this DOESN'T CONSUME the grid pointer grid. 
GridStateNode* map_lookup_with_insert(GridStateMap* map, Grid const * const grid, bool insert, WinState state) {
    size_t hash = hash_grid(*grid);
    
    for (GridStateNode* pt = map->data[hash]; pt != nullptr; pt = pt->next) {
        if (equals(grid, pt->grid)) {
            
            return pt;
        }
    }
    ;
    GridStateNode * ret = nullptr;

    if (insert && (ret = malloc(sizeof(GridStateNode))) && (ret->grid = copy(grid))) {
        ret->state = state;
        ret->next = map->data[hash];
        map->data[hash] = ret; //appended it to the front
    } else {
        free(ret);
        ret = nullptr;
    }
    return ret;
}


//Allocates new Grid list to hold all possible moves.

//Should make this return list with grid pointers to hashmap??
//Then it doesn't necessarily reallocates.
//Use map insert!
//It will create the node in the map,
//then builds the GridList using the grids that are held in the map
GridList* find_possible_moves_into_map(GridStateMap* map, Grid const* current_grid) {
    GridList* current_list = nullptr;
    //holds space for a temp object
    GridList* temp_list = nullptr;
    
    Grid temp_grid;
    //Initializes temp by copying the current_grid.
    copy_grid_into(current_grid, &temp_grid);

    Player const current_player = current_grid->player;
    Player const other_player = next_player(current_player);

    //temp_grid will contain the next moves, so we record the next player
    temp_grid.player = other_player;
    //Right now temp grid is in an invalid position: wrong player to move. 

    GridStateNode* node = nullptr;

    if (is_full(current_grid)) {
        return nullptr; //No next states
    }
    for (size_t i = 0; i < GRID_TOTAL; i++) {
        if (temp_grid.data[i] == EMPTY) {
            //Move to the empty tile with player
            temp_grid.data[i] = current_player; 
            //We changed temp_grid to a valid position
            //Add into the map with sentinel value, if it hasn't been yet.
            //This allocates space for a new grid object in the map.
            //This only borrows temp_grid.

            node = map_lookup_with_insert(map, &temp_grid, true, UNKNOWN);


            if (!node) {
                destroy_grid_list_keep_grids(current_list);
                return nullptr;
            } 
            //Change temp back
            temp_grid.data[i] = EMPTY;
            //If allocation error
            
            //Borrows the grid from the map
            if (temp_list = malloc(sizeof(GridList))){
                init_from_grid(temp_list, node->grid);
                temp_list->next = current_list;
                current_list = temp_list;
        
            } else {
                //allocation error!
                //cleanup code
                //We DO NOT free the grid, as it's the map's responsibility. 
                destroy_grid_list_keep_grids(current_list);
                return nullptr;
            }            
        }
    }
    return current_list;
}



//Populates the map with the given starting grid

//Do breadth first search: add the possible moves to the end. 
WinState calculate_position(GridStateMap* map, Grid const * const start_grid) {
    GridList* to_calculate = new_grid_copy(start_grid);

    GridList* current_node = nullptr; //Used to pop to_calculate

    GridList* end = to_calculate;

    Grid const* current_grid;
    Player current_player;
    while(to_calculate != nullptr) {
        current_node = to_calculate;

        current_grid = current_node->grid;
        current_player = current_grid->player;
        //Check if current is an ended game:
        //Or if the position has been calculated already

        GridStateNode* map_node = map_lookup_with_insert(map, current_grid, true, UNKNOWN);
        //Only inserts if we cannot find it.
        if (!map_node) {
            //Error checking! abort and clean up.
            //printf("Allocation error\n");
            while (to_calculate != nullptr) {
                current_node = to_calculate;
                to_calculate = to_calculate->next;
                free(current_node);
            }
        }
        //If we haven't seen it before!
        if (map_node->state == UNKNOWN) {
            Player pot_winner = has_won(current_grid);
            //printf("Potential winner: %s\n", player_to_string(pot_winner));
            //print_grid(map_node->grid);
            if (pot_winner != EMPTY) {
                map_node->state = (WinState) pot_winner; //This is just an integer cast.

        
            } else if (is_full(current_grid)) {
                //Draw condition
                //printf("Drawing position: \n");
                //print_grid(map_node->grid);
                map_node->state = DRAW;
            }
        } 
        //This happens if we have calculated the node before, or
        //if we just assigned it a value.
        if (map_node->state != UNKNOWN) {
            //Pop the current one from list, we're done processing it.
            //We ONLY free the pointer at to_calculate, DO NOT DESTROY GRID
            //As the grid is in the map
            //printf("State is %s\n", state_to_string(map_node->state));
            //print_grid(map_node->grid);
            to_calculate = to_calculate->next;
            free(current_node);
            //No need for extra processing.
            continue;
        }

        //Otherwise, we need to process all the possible moves from the current position. 
        GridList* possible_moves = find_possible_moves_into_map(map, current_grid);

        //Add the possible moves to our map (giving it the ownership!!)
        //As well as process if the next possible moves are all processed

        //If we see a winning state (so a losing state for the next player), it's a win.

        //If all are losses, it's a loss

        //else it's a draw. 
        bool is_win = false;
        bool add_to_list = false;
        bool all_losses = true;
        GridList* prev = nullptr;

        GridStateNode* iter_node = nullptr;

        for (GridList* iter = possible_moves; iter != nullptr; iter = iter->next) {
            //This isn't hit at the last iteration when iter == nullptr
            prev = iter;

            iter_node = map_lookup_with_insert(map, iter->grid, false, UNKNOWN);

            if (iter_node->state == (WinState) current_player) {
                //If we see a winning state (so a losing state for the next player), it's a win.
                map_node->state = (WinState) current_player;
                is_win = true; //Pop the current grid from list
                all_losses = false;
                add_to_list = false;
                break;
            } else if (add_to_list) {
                continue;
            } else if (iter_node->state == UNKNOWN) {
                //We need more processing
                //Add list to the front of to_calculate
                all_losses = false;
                add_to_list = true;
                continue; //We want to loop to the end of the list anyway here
            } else if (iter_node->state == DRAW) {
                //At least one draw, so it isn't a loss
                all_losses = false;
            }
        }
        //If we find a win, there is no need to calculate the other positions!
        if (is_win) {
            to_calculate = to_calculate->next;

            free(current_node);

            //printf("Current move %s, state: %s\n", player_to_string(current_grid->player), state_to_string(map_node->state));
            //print_grid(current_grid);

        } else if (add_to_list) {
            //Links the lists, as there are unprocessed things.
            
            end->next = possible_moves;
            prev->next = current_node;

            to_calculate = to_calculate->next;

            //Re use current_node
            current_node->next = nullptr;
            end = current_node;

            
        } else if (all_losses) {
            //Loss condition
            map_node->state = (WinState) next_player(current_player);
            to_calculate = to_calculate->next;

            free(current_node);
            //printf("Current move %s, state: %s\n", player_to_string(current_grid->player), state_to_string(map_node->state));
            //print_grid(current_grid);

        } else {
            //Draw condition. Not any of the previous: either a win or all losses.
            map_node->state = DRAW;
            to_calculate = to_calculate->next;

            free(current_node);
            //printf("Current move %s, state: %s\n", player_to_string(current_grid->player), state_to_string(map_node->state));
            //print_grid(current_grid);
        }
        //Finally pop, cleanup of current_node done. 
    }
    return map_lookup_with_insert(map, start_grid, false, UNKNOWN)->state;
}


/*
Returns an integer 0 <= t <= 8 for the location of the next best move.
Assumes the win states have been calculated already. 
*/

size_t best_move_from_map(GridStateMap * map, Grid const * const grid) {
    
    GridStateNode* map_node = map_lookup_with_insert(map, grid, false, UNKNOWN);

    if (!map_node || map_node->state == UNKNOWN) {
        return GRID_TOTAL; //Error condition: we must have generated the map already.
    }

    Player player = grid->player; //The player we're finding the best move for.
    WinState target_state = map_node->state; //This is the state we're looking for.  


    //If we have a losing board
    if (target_state == (WinState) next_player(player)) {
        //Check best squares on the board.
        //Middle square is best
        if (get(grid, 1, 1) == EMPTY) {
            return get_index(1, 1);
        } 
        //Next do corner squares
        else if (get(grid, 0, 0) == EMPTY) {
            return get_index(0, 0);
        } else if (get(grid, 2, 0) == EMPTY) {
            return get_index(2, 0);
        } else if (get(grid, 0, 2) == EMPTY) {
            return get_index(0, 2);
        } else if (get(grid, 2, 2) == EMPTY) {
            return get_index(2, 2);
        }
        //Last just check the rest of them
        else if (get(grid, 1, 0) == EMPTY) {
            return get_index(1, 0);
        } else if (get(grid, 1, 2) == EMPTY) {
            return get_index(1, 2);
        } else if (get(grid, 0, 1) == EMPTY) {
            return get_index(0, 1);
        } else if (get(grid, 2, 1) == EMPTY) {
            return get_index(2, 1);
        } else {
            return GRID_TOTAL; //This is returned if the board is full. 
        }
    } 

    //Otherwise: We loop through the possible moves for lower overhead
    //Temp holds the current new move we're looking at.
    Grid temp;

    copy_grid_into(grid, &temp);
    temp.player = next_player(player);

    //State should be either our player or is DRAW. 

    GridStateNode* iter_node = nullptr;

    //If the board is a draw or win:


    for (size_t i = 0; i < GRID_TOTAL; i++) {
        if (grid->data[i] == EMPTY) {
            temp.data[i] = player;

            iter_node = map_lookup_with_insert(map, &temp, false, UNKNOWN);

            temp.data[i] = EMPTY; //Reset temp

            if (!iter_node || iter_node->state == UNKNOWN) {
                return GRID_TOTAL; //Again, this is an error condition. Map wasn't sufficiently generated
            }

            //Now we calculate the logic

            //If we find a condition matching the target state, return this as the move
            if (iter_node->state == target_state) {
                return i;
            }
        }
    }

    return GRID_TOTAL; //This is an error condition: couldn't find win or draw??
}


/*
TODO need to integrate the search function.
*/
int main() {

    Grid BOARD;
    Grid* bpt = reset(&BOARD);

    BOARD.player = X_PL;
    char line[LINE_MAX];

    size_t move_x = 0;
    size_t move_y = 0;
    Player current_player = X_PL;

    printf("Would you like to play with a computer? (y/n) \n");

    char computer = '\0';
    while(true) {
        if (fgets(line, sizeof(line), stdin)) {
            if (sscanf(line, "%c", &computer) == 1) {
                if (computer == 'Y' || computer == 'N' || computer == 'y' || computer == 'n') {
                    break;
                } 
            } 
            //Error input
            printf("Please input either y or n.\n");
        }
    }
    //Loop with two human players
    if (computer == 'n' || computer == 'N') {
        while(true) {
            printf("Current grid: \n");
            print_grid(bpt);
            printf("It's player %s's turn! Make a move. \n", player_to_string(current_player));
            if (fgets(line, sizeof(line), stdin)) {
                if (sscanf(line, "%d %d", &move_x, &move_y) == 2 && get(bpt, move_x, move_y) == EMPTY) {
                    move(bpt, move_x, move_y);
                    //Switches current player
                
                    //Here are the end states:
                    if (is_winning_move(bpt, move_x, move_y)) {
                        printf("Player %s won! \n", player_to_string(current_player));
                        print_grid(bpt);
                        break;
                    } else if (is_full(bpt)) {
                        printf("It's a draw!\n");
                        print_grid(bpt);
                        break;
                    }
                    //Advances current player
                    current_player = BOARD.player;
                    continue;
                } else {

                    printf("Illegal move or failed read. Enter a move as x y, with 0<=x,y<=3.\n");
                }
            } else {
                printf("Failed to read line. Enter a move as x y, with 0<=x,y<=3.\n");
            }
        }
    } 

    //Loop to play with computer
    else {

        GridStateMap MAP;
        GridStateMap* mpt = init_map(&MAP); //Initializes the map

        //Asks to go first or second

        int response = 0;
        Player computer_player = EMPTY;

        printf("Would you like to go first or second? (1/2)\n");
        while(true) {
            if (fgets(line, sizeof(line), stdin)) {
                if (sscanf(line, "%d ", &response) == 1) {
                    if (response == 1) {
                        computer_player = O_PL;
                        break;
                    } else if (response == 2) {
                        computer_player = X_PL;
                        break;
                    }
                }
                printf("Please input either 1 or 2\n");
            }
        }

        //Print the empty board once:
        //Always take the center if possible
        if (computer_player == X_PL) {
            printf("Current grid: \n");
            print_grid(bpt);
            move(bpt, 1, 1);
        }
        //Populates the map.
        calculate_position(mpt, bpt);
        while(true) {
            printf("Current grid: \n");
            print_grid(bpt);
            printf("It's your turn! Make a move. \n");
            if (fgets(line, sizeof(line), stdin)) {
                if (sscanf(line, "%d %d", &move_x, &move_y) == 2 && get(bpt, move_x, move_y) == EMPTY) {
                    move(bpt, move_x, move_y);
                    printf("Current grid: \n");
                    print_grid(bpt);
                    //Here are the end states:
                    if (is_winning_move(bpt, move_x, move_y)) {
                        printf("You won! \n");
                        print_grid(bpt);
                        break;
                    } else if (is_full(bpt)) {
                        printf("It's a draw!\n");
                        print_grid(bpt);
                        break;
                    }

                    //Find computer move now
                    size_t move = best_move_from_map(mpt, bpt);
                    if (!(move < GRID_TOTAL)) {
                        printf("Error, lookup failed.");
                        return EXIT_FAILURE;
                    }
                    BOARD.data[move] = current_player;
                    BOARD.player = next_player(BOARD.player);

                    if (has_won(bpt)) {
                        printf("You lost!");
                        print_grid(bpt);
                        break;
                    } else if (is_full(bpt)) {
                        printf("It's a draw!\n");
                        print_grid(bpt);
                        break;
                    }
                    continue;
                } 
            } 
            printf("Failed to read line. Enter a move as x y, with 0<=x,y<=3.\n");
        }





    }
    return EXIT_SUCCESS;
    
}

