#ifndef map_h
#define map_h

#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>

#include "utility.h"

// IF YOU WANT TO INCREASE YOUR MAP SIZE OR ENEMY SIZE MAKE SURE TO INCREASE DEFINE VARIABLE BELOW IF IT EXCEED THE MAXIMUM
#define MAX_MAP_ROW 100
#define MAX_MAP_COL 100
#define MAX_ENEMY_SPAWN 100

/*
    Map Tiles type
    Hint : Some type might need a background, to configure it try to modify "get_map_offset(Map * map)"
*/
typedef enum _BLOCK_TYPE{
    FLOOR,
    WALL,
    DOOR_CLOSE,
    HOLE,
    COIN,
    NOTHING,
    TROPHY,
    LADDER,
    DOOR,
    DOORF,
    BUTTON,
    DUMMYFLOOR
} BLOCK_TYPE;

typedef enum _COIN_STATUS {
    APPEAR,
    DISAPPEARING,
    DISAPPEARED
} COIN_STATUS;


// Map Struct
typedef struct Map_{
    uint8_t ** map; // Map array
    Point ** offset_assets; // Assets
    
    int row, col;
    
    // Map assets
    ALLEGRO_BITMAP* assets;
    ALLEGRO_BITMAP* extra_assets;
    ALLEGRO_BITMAP* coin_assets;
    ALLEGRO_BITMAP* trophy_asset;
    ALLEGRO_BITMAP* door_assets;
    ALLEGRO_BITMAP* button_assets;

    // Button&Door Properties
    int button_identity[MAX_MAP_ROW][MAX_MAP_COL]; // integer
    int button_status[MAX_MAP_ROW][MAX_MAP_COL]; // 0 for unpressed, 1 for pressed
    int door_identity[MAX_MAP_ROW][MAX_MAP_COL];
    int door_status[MAX_MAP_ROW][MAX_MAP_COL]; // 0 for close, 1 for opened


    // Coin Properties
    ALLEGRO_SAMPLE* coin_audio;
    COIN_STATUS coin_status[MAX_MAP_ROW][MAX_MAP_COL];
    int cointick[MAX_MAP_ROW][MAX_MAP_COL];

    // Spawn Coordinate
    Point Spawn;
    Point EnemySpawn[MAX_ENEMY_SPAWN];
    char EnemyCode[MAX_ENEMY_SPAWN];
    uint8_t EnemySpawnSize;

} Map;

/*
    MAP FUNCTION
    Feel free to add more if you have some idea or interaction with the map
 */
Map create_map(char * path, uint8_t type); // Create a map based on given file path
void draw_map(Map * map, Point cam); // Draw the map
void update_map(Map * map, Point player_coord, int * total_coins); // Update map : you might want add some parameter here
void destroy_map(Map * map); // Destroy map

bool isWalkable(BLOCK_TYPE block);

#endif /* map_h */
