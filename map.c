#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "map.h"
#include "win_scene.h"
#include "game_scene.h"
/*
    [OFFSET CALCULATOR FUNCTIONS]
      You might want to modify this functions if you are using different assets
      In short, this functions help us to find which asset to draw
 */
static void get_map_offset(Map * map);
static Point get_wall_offset_assets(Map * map, int i, int j);
static Point get_floor_offset_assets(Map * map, int i, int j);
static Point get_hole_offset_assets(Map * map, int i, int j);
static const int offset = 16;

static bool tile_collision(Point player, Point tile_coord);

int animationtick = 0;
int gobkingsurprise;
int buttoncount = 1;
int doorcount = 1;
int dooranimationtick = 0;
extern int enemycount;

Map create_map(char * path, uint8_t type){
    Map map;

    animationtick = 0;
    gobkingsurprise = 1;
    memset(&map, 0, sizeof(Map));
    
    FILE * f = fopen(path, "r");
    if(!f){
        game_abort("Can't load map file : %s", path);
    }
    
    game_log("Using %s to load the map", path);
    
    fscanf(f, "%d %d", &map.row, &map.col);
    char ch;
    
    // Map Array
    map.map = (uint8_t **) malloc(map.row * sizeof(uint8_t *));
    for(int i=0; i<map.row; i++){
        map.map[i] = (uint8_t *) malloc(map.col * sizeof(uint8_t));
    }
    
    // Map Offset Resource Pack Array
    map.offset_assets = (Point **) malloc(map.row * sizeof(Point *));
    for(int i=0; i<map.row; i++){
        map.offset_assets[i] = (Point *) malloc(map.col * sizeof(Point));
    }
    

    // Scan the map to the array
    int door_counter = 0;
    for(int i = 0; i < map.row; i++){
        for(int j = 0; j < map.col; j++){
            fscanf(f, " %c", &ch);

            switch(ch)
            {
                case '#': // Case Wall
                    map.map[i][j] = WALL;
                    break;
                case '.': // Case Floor
                    map.map[i][j] = FLOOR;
                    break;
                case 'L': // Ladder
                    map.map[i][j] = LADDER;
                    break;

                case 'D': // DOOR
                    map.map[i][j] = DOOR;
                    map.door_status[i][j] = 0;
                    if (map.map[i][j - 1] == DOOR) {
                        doorcount -= 1;
                        map.door_identity[i][j] = doorcount++;
                    }
                    else {
                        map.door_identity[i][j] = doorcount++;
                    }
                    break;
                case 'B': // BUTTON
                    map.map[i][j] = BUTTON;
                    map.button_status[i][j] = 0; 
                    map.button_identity[i][j] = buttoncount++;
                    break;

                case 'P': // Spawn Point
                    map.map[i][j] = FLOOR;
                    map.Spawn = (Point){i, j};
                    break;

                case 'S': // Slime Enemy
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){i, j};
                    enemycount++;
                    break;
                case 'G': // GOBLIN
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    enemycount++;
                    break;
                case 'K': //skeleton
                    map.map[i][j] = FLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    enemycount++;
                    break;
                case '~':
                    map.map[i][j] = DUMMYFLOOR;
                    break;
                case 'Y': // store
                    map.map[i][j] = DUMMYFLOOR;
                    map.EnemyCode[map.EnemySpawnSize] = ch;
                    map.EnemySpawn[map.EnemySpawnSize++] = (Point){ i, j };
                    break;

                case 'C': // Coins
                    map.map[i][j] = COIN;
                    break;

                case '_': // Nothing
                    map.map[i][j] = HOLE;
                    break;
                case 'T': // Trophy
                    map.map[i][j] = TROPHY;
                    break;

                default: // If not determined, set it as black
                    map.map[i][j] = NOTHING;
                    break;
            }
        }
    }
    
    map.assets = al_load_bitmap("Assets/map_packets.png");
    if(!map.assets){
        game_abort("Can't load map assets");
    }

    map.coin_assets = al_load_bitmap("Assets/coins.png");
    if (!map.coin_assets) {
        game_abort("Can't load coin assets");
    }
    
    map.door_assets = al_load_bitmap("Assets/Door.png");
    map.trophy_asset = al_load_bitmap("Assets/trophy.png");
    map.extra_assets = al_load_bitmap("Assets/test_tileset.png");
    map.button_assets = al_load_bitmap("Assets/Button.png");

    // load the offset for each tiles type
    get_map_offset(&map);

    map.coin_audio = al_load_sample("Assets/audio/coins.mp3");
    if(!map.coin_audio){
        game_abort("Can't load coin audio");
    }

    fclose(f);
    
    return map;
}

int offsetx = 0, offsety = 0;
void draw_map(Map * map, Point cam){
    // Draw map based on the camera point coordinate
    al_clear_to_color(al_map_rgb(24, 20, 37));

    for(int i=0; i<map->row; i++){
        for(int j=0; j<map->col; j++){
            
            int dy = i * TILE_SIZE - cam.y; // destiny y axis
            int dx = j * TILE_SIZE - cam.x; // destiny x axis
            
            Point offset_asset = map->offset_assets[i][j];
            
            al_draw_scaled_bitmap(map->assets, // image
                                  offset_asset.x, offset_asset.y, 16, 16, // source x, source y, width, height
                                  dx, dy, TILE_SIZE, TILE_SIZE, // destiny x, destiny y, destiny width, destiny height
                                  0 // flag : set 0
                                  );

            switch(map->map[i][j]){
                case COIN:
                    if (map->coin_status[i][j] == APPEAR) {
                        offsety = 0;
                        offsetx = 16 * (map->cointick[i][j] / 8);
                    }
                    else if (map->coin_status[i][j] == DISAPPEARING) {
                        offsety = 16;
                        offsetx = 16 * (map->cointick[i][j] / 8);

                        if (map->cointick[i][j] == 63) {
                            map->coin_status[i][j] = DISAPPEARED;
                            map->map[i][j] = FLOOR;
                        }
                    }
                    al_draw_scaled_bitmap(map->coin_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                case TROPHY:
                    offsetx = 32 * (int)(animationtick / 10);
                    offsety = 0;
                    al_draw_scaled_bitmap(map->trophy_asset,
                        offsetx, offsety, 32, 32,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                case LADDER:
                    offsety = 16 * 3;
                    offsetx = 16 * 9;
                    al_draw_scaled_bitmap(map->extra_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                case DOOR:
                    offsety = 0;
                    offsetx = 0;
                    if (map->door_status[i][j] == 1) {
                        offsetx = 32 * (dooranimationtick / 10);
                        if (dooranimationtick == 69) {
                            map->map[i][j] = DOORF;
                        }
                    }
                    al_draw_scaled_bitmap(map->door_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                case DOORF:
                    offsety = 0;
                    offsetx = 32 * 6;
                    al_draw_scaled_bitmap(map->door_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                case BUTTON:
                    offsety = 0;
                    offsetx = 0;
                    //add animation if you want to kill yourself
                    if (map->button_status[i][j] == 1) {
                        offsetx = 32;
                    }
                    al_draw_scaled_bitmap(map->button_assets,
                        offsetx, offsety, 16, 16,
                        dx, dy, TILE_SIZE, TILE_SIZE,
                        0);
                    break;
                default:
                    break;
            }
        /*#ifdef DRAW_HITBOX
            al_draw_rectangle(
                dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
                al_map_rgb(0, 255, 0), 1
            );
        #endif*/
        }
    }
}

extern int pause;
extern int nextlevelprompt;
void update_map(Map * map, Point player_coord, int* total_coins){
    /*
        Hint: To check if it's collide with object in map, you can use tile_collision function
        e.g. to update the coins if you touch it
    */

    //animation thingy
    animationtick = (animationtick+1) % 100;
    dooranimationtick = (dooranimationtick + 1) % 70;

    for (int i = 0; i < map->row; i++)
        for (int j = 0; j < map->col; j++)
            if (map->map[i][j] == COIN) map->cointick[i][j] = (map->cointick[i][j] + 1) % 64;

    //collision
    int middlex = (player_coord.x + TILE_SIZE / 2) / TILE_SIZE;
    int middley = (player_coord.y + TILE_SIZE / 2) / TILE_SIZE;

    if (tile_collision && 
        map->map[middley][middlex] == COIN &&
        map->coin_status[middley][middlex] == APPEAR) {
        map->coin_status[middley][middlex] = DISAPPEARING;
        al_play_sample(map->coin_audio, SFX_VOLUME, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
        (*total_coins)++;
        map->cointick[middley][middlex] = 0;
    }

    if (tile_collision &&
        map->map[middley][middlex] == LADDER) {
        nextlevelprompt = 1;
        pause = 1;
    }

    if (tile_collision &&
        map->map[middley][middlex] == TROPHY) {
        change_scene(create_win_scene());
        return;
    }

    if (tile_collision &&
        map->map[middley][middlex] == TROPHY) {
        change_scene(create_win_scene());
        return;
    }

    if (tile_collision &&
        map->map[middley][middlex] == BUTTON) {
        map->button_status[middley][middlex] = 1;
        for (int i = 0; i < map->row; i++) {
            for (int j = 0; j < map->col; j++) {
                if (map->door_identity[i][j] == map->button_identity[middley][middlex] && map->door_status[i][j] == 0) {
                    map->door_status[i][j] = 1;
                    dooranimationtick = 0;
                }
            }
        }
        return;
    }
}

void destroy_map(Map * map){
    for(int i = 0; i < map->row; i++){
        free(map->map[i]);
        free(map->offset_assets[i]);
    }
    free(map->map);
    free(map->offset_assets);

    al_destroy_bitmap(map->assets);
    al_destroy_bitmap(map->coin_assets);
    al_destroy_sample(map->coin_audio);
    al_destroy_bitmap(map->trophy_asset);
}

bool isWalkable(BLOCK_TYPE block){
    if (block == FLOOR  || 
        block == COIN   || 
        block == TROPHY || 
        block == LADDER || 
        block == BUTTON ||
        block == DOORF) {
        return true;
    }
    return false;
}

/*
    DON'T CHANGE CODE BELOW UNLESS YOU ARE UNDERSTAND WELL
    OR WANT TO CHANGE TO DIFFERENT ASSETS
 */
static bool isWall(Map * map, int i, int j);
//static bool isFloorHole(Map * map, int i, int j);
static bool isFloor(Map * map, int i, int j);
static bool isNothing(Map * map, int i, int j);

static Point get_floor_offset_assets(Map * map, int i, int j){
    bool up = isWall(map, i-1, j);
    bool left = isWall(map, i, j-1);
    bool right = isWall(map, i, j+1);
    bool top_left = isWall(map, i-1, j-1);
    bool top_right = isWall(map, i-1, j+1);
    
    if(up && left && right){
        return (Point){ offset * 12, offset * 3 };
    }
    if(up && left){
        return (Point){ offset * 11, 0 };
    }
    if(up && right){
        return (Point){ offset * 13, 0 };
    }
    if(left){
        if(top_left)
            return (Point){ offset * 11, offset * 1 };
        else
            return (Point){ offset * 15, offset * 1 };
        
    }
    if(right){
        if(top_right)
            return (Point){ offset * 13, offset * 1 };
        else
            return (Point){ offset * 14, offset * 1 };
    }
    if(up){
        return (Point){ offset * 12, 0 };
    }
    if(top_left){
        return (Point){ offset * 11, offset * 3 };
    }
    if(top_right){
        return (Point){ offset * 13, offset * 3 };
    }
    
    return (Point){12 * offset, 4 * offset};
}

// Calculate the offset from the source assets
static Point get_wall_offset_assets(Map * map, int i, int j){
    bool up = isWall(map, i-1, j);
    bool down = isWall(map, i+1, j);
    bool left = isWall(map, i, j-1);
    bool right = isWall(map, i, j+1);

    if(up && down && left && right){
        return (Point){ 3 * offset, 5 * offset };
    }
    if(up && down && right){
        bool left_floor = isFloor(map, i, j - 1);
        bool right_down = isWall(map, i + 1, j + 1);
        bool right_up = isWall(map, i - 1, j + 1);
        if (right_down && right_up)
            return (Point) { 2 * offset, 5 * offset };
        if (left_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 4 * offset, 5 * offset };
        }
    }
    if (up && down && left) {
        bool right_floor = isFloor(map, i, j + 1);
        bool left_down = isWall(map, i + 1, j - 1);
        bool left_up = isWall(map, i - 1, j - 1);
        if (left_down && left_up)
            return (Point) { 4 * offset, 5 * offset };
        if (right_floor) {
            return (Point) { 1 * offset, 3 * offset };
        }
        else {
            return (Point) { 2 * offset, 5 * offset };
        }
    }
    if(down && right && left){
        bool down_right_wall = isWall(map, i + 1, j + 1);
        bool down_left_wall = isWall(map, i + 1, j - 1);
        bool down_right = isFloor(map, i+1, j+1);
        bool down_left = isFloor(map, i+1, j-1);
        if(down_right_wall && down_left_wall)
            return (Point) { 3 * offset, 4 * offset };
        if((down_right ^ down_left) == 0)
            return (Point){ 8 * offset, 5 * offset };
        if(down_right)
            return (Point){ 4 * offset, 4 * offset };
        if(down_left)
            return (Point){ 2 * offset, 4 * offset };
    }
    if(left && right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down && up){
        bool left_ = isFloor(map, i, j-1);
        bool right_ = isFloor(map, i, j+1);
        if((left_ ^ right_) == 0)
            return (Point){ 1 * offset, 3 * offset};
        if(left_)
            return (Point){ 2 * offset, 5 * offset};
        return (Point){ 4 * offset, 5 * offset};
    }
    if(down && right){
        if(isFloor(map, i+1, j+1))
            return (Point){ 4 * offset, 5 * offset };
        else
            return (Point){ 2 * offset, 4 * offset };
    }
    if(down && left){
        if (isFloor(map, i, j + 1) && isFloor(map, i + 1, j - 1))
            return (Point) { 1 * offset, 2 * offset };
        else if(isFloor(map, i+1, j-1))
            return (Point){ 2 * offset, 5 * offset };
        else
            return (Point){ 4 * offset, 4 * offset};
    }
    if(up && right){
        if(isFloor(map, i+1, j))
            return (Point){ 2 * offset, 6 * offset};
        else
            return (Point){ 5 * offset, 6 * offset};
    }
    if(up && left){
        if(isFloor(map, i+1, j))
            return (Point){ 4 * offset, 6 * offset};
        else
            return (Point){ 6 * offset, 6 * offset};
    }
    if(left || right){
        if(isFloor(map, i+1, j))
            return (Point){ 7 * offset, 5 * offset};
        else
            return (Point){ 7 * offset, 7 * offset};
    }
    if(down){
        return (Point){ 1 * offset, 2 * offset};
    }
    if(up){
        return (Point){ 1 * offset, 4 * offset};
    }
    
    return (Point){ 0, 0 };
}

static Point get_hole_offset_assets(Map * map, int i, int j){
    bool up = isNothing(map, i-1, j);
    bool U_Floor = isFloor(map, i-1, j);
    
    if(up){
        return (Point){ 0, 0 };
    }
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static Point get_nothing_offset_assets(Map * map, int i, int j){
    bool U_Floor = isFloor(map, i-1, j);
    if(U_Floor){
        bool UL = isFloor(map, i-1, j-1);
        bool UR = isFloor(map, i-1, j+1);
        if(UL && UR) return (Point){ offset * 13, offset * 11 };
        if(UL) return (Point){ offset * 14, offset * 11 };
        if(UR) return (Point){ offset * 12, offset * 11 };
        return (Point){ offset * 10, offset * 12 };
    }
    return (Point){ 0, 0 };
}

static void get_map_offset(Map * map){
    // Calculate once to use it later on when draw() function called
    for(int i = 0; i < map->row; i++){
        for(int j = 0; j < map->col; j++){
            switch(map->map[i][j]){
                case WALL:
                    map->offset_assets[i][j] = get_wall_offset_assets(map, i, j);
                    break;
                case DOOR:

                case FLOOR:

                case DUMMYFLOOR:

                case LADDER:

                case COIN:

                case TROPHY:
                    map->offset_assets[i][j] = get_floor_offset_assets(map, i, j);
                    break;
                case HOLE:
                    map->offset_assets[i][j] = get_hole_offset_assets(map, i, j);
                    break;
                case NOTHING:

                default:
                    map->offset_assets[i][j] = (Point){ 0, 0 };
                    break;
            }
        }
    }
}

static bool isWall(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == WALL) return true;
    return false;
}

static bool isFloor(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return false;
    if(map->map[i][j] == FLOOR) return true;
    return false;
}

static bool isNothing(Map * map, int i, int j){
    if(i < 0 || j < 0 || i >= map->row || j >= map->col) return true;
    if(map->map[i][j] == NOTHING || map->map[i][j] == HOLE) return true;
    return false;
}

/*
    Collision
 */
static bool tile_collision(Point player, Point tile_coord){
    if (player.x < tile_coord.x + TILE_SIZE &&
        player.x + TILE_SIZE > tile_coord.x &&
        player.y < tile_coord.y + TILE_SIZE &&
        player.y + TILE_SIZE > tile_coord.y) {
            return true; // Rectangles collide
    } else {
        return false; // Rectangles do not collide
    }
}
