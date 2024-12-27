#include "player.h"
#include "game.h"
#include "utility.h"
#include "map.h"

#include <math.h>

static bool isCollision(Player* player, Map* map);
int flag = 0;
int delay = 0;

extern int remaininghp;
extern int startwithsix;
extern int secondlife;
Player create_player(char * path, int row, int col){
    Player player;
    memset(&player, 0, sizeof(player));
    
    player.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    player.speed = 4;
    player.health = remaininghp;
    if (startwithsix) {
        player.maxhealth = 6;
    }
    else {
        player.maxhealth = 5;
    }
    player.dyinganimationcounter = 0;
    player.image = al_load_bitmap(path);
    if(!player.image){
        game_abort("Error Load Bitmap with path : %s", path);
    }
    
    return player;
}

void update_player(Player * player, Map* map){
    Point original = player->coord;
    if (player->status == PLAYER_DYING) {
        player->animation_tick = (player->dyinganimationcounter++) % 128;
        if (player->dyinganimationcounter >= 128) {
            player->status = PLAYER_DEAD;
        }
        return;
    }

    player->status = PLAYER_IDLE;
    flag = (mouseState.x < SCREEN_W / 2) ? 0 : 1; // Change the flag to flip character

    // Knockback effect,
    // if you're reading this, change some things here for slower attacks(e.g. bigskelly)
    if(player->knockback_CD > 0 && !delay){

        player->knockback_CD--;
        int next_x = player->coord.x + player->speed * cos(player->knockback_angle);
        int next_y = player->coord.y + player->speed * sin(player->knockback_angle);
        Point next;
        next = (Point){next_x, player->coord.y};
        
        if(!isCollision(player, map)){
            player->coord = next;
        }
        
        next = (Point){player->coord.x, next_y};
        if(!isCollision(player, map)){
            player->coord = next;
        }
    }
    if (delay > 0) {
        delay--;
    }
    /*
        [TODO HACKATHON 1-1] 
        
        Player Movement
        Adjust the movement by player->speed

        if (keyState[ALLEGRO_KEY_W]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_S]) {
            player->coord.y = ...
            player->direction = ...
        }
    */
    
    if (keyState[ALLEGRO_KEY_W] && !delay && !player->knockback_CD) {
        player->coord.y -= player->speed;
        player->direction = UP;
        player->status = PLAYER_WALKING;
    }
    if (keyState[ALLEGRO_KEY_S] && !delay && !player->knockback_CD) {
        player->coord.y += player->speed;
        player->direction = DOWN;
        player->status = PLAYER_WALKING;
    }

    // if Collide, snap to the grid to make it pixel perfect
    if(isCollision(player, map)){
        player->coord.y = round((float)original.y / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO HACKATHON 1-2] 
        
        Player Movement
        Add/Subtract the movement by player->speed

        if (keyState[ALLEGRO_KEY_A]) {
            player->coord.y = ...
            player->direction = ...
        }
        if (keyState[ALLEGRO_KEY_D]) {
            player->coord.y = ...
            player->direction = ...

    }
    */

    if (keyState[ALLEGRO_KEY_A] && !delay && !player->knockback_CD) {
        player->coord.x -= player->speed;
        player->direction = LEFT;
        player->status = PLAYER_WALKING;
    }
    if (keyState[ALLEGRO_KEY_D] && !delay && !player->knockback_CD) {
        player->coord.x += player->speed;
        player->direction = RIGHT;
        player->status = PLAYER_WALKING;
    }

    if(isCollision(player, map)){
        player->coord.x = round((float)original.x / (float)TILE_SIZE) * TILE_SIZE;
    }
    
    /*
        [TODO Homework] 
        
        Calculate the animation tick to draw animation later
    */
    player->animation_tick = (player->animation_tick + 1)%64;

    if (secondlife) {
        player->image = al_load_bitmap("Assets/panda.png");
    }
    else {
        player->image = al_load_bitmap("Assets/panda2.png");
    }
}

void draw_player(Player * player, Point cam){
    int dy = player->coord.y - cam.y; // destiny y axis
    int dx = player->coord.x - cam.x; // destiny x axis
    
    /*
        [TODO Homework] 
        
        Draw Animation of Dying, Walking, and Idle
    */
    int offsetx, offsety;

    if (player->status == PLAYER_WALKING) {
        offsety = 32;
        offsetx = 32 * (player->animation_tick / 16);
    }
    else if (player->status == PLAYER_IDLE) {
        offsety = 0;
        offsetx = 32 * (player->animation_tick / 32);
    }
    else if (player->status == PLAYER_DYING ) {
        offsety = 64;
        offsetx = 32 * (player->animation_tick / 32);
        al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 255, 255),
            offsetx, offsety, 32, 32,
            dx, dy, TILE_SIZE, TILE_SIZE,
            flag
        );
        return;
    }
    else {
        return;
    }

    if (player->knockback_CD > 16 && !delay) {
        if (player->knockback_CD % 5 < 3) {
            al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 0, 0),
                offsetx, offsety, 32, 32,
                dx, dy, TILE_SIZE, TILE_SIZE, 
                flag 
            );
        }
        else {
            al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(230, 230, 230),
                offsetx, offsety, 32, 32, 
                dx, dy, TILE_SIZE, TILE_SIZE,
                flag // Flip or not
            );
        }
    }
    else {
        al_draw_tinted_scaled_bitmap(player->image, al_map_rgb(255, 255, 255),
            offsetx, offsety, 32, 32, 
            dx, dy, TILE_SIZE, TILE_SIZE,
            flag
        );
    }
    
//#ifdef DRAW_HITBOX
//    al_draw_rectangle(
//        dx, dy, dx + TILE_SIZE, dy + TILE_SIZE,
//        al_map_rgb(255, 0, 0), 1
//    );
//#endif
    
}

void delete_player(Player * player){
    al_destroy_bitmap(player->image);
}

static bool isCollision(Player* player, Map* map){

    if (player->coord.x < 0 ||
        player->coord.y < 0 ||
        (player->coord.x + TILE_SIZE - 1) / TILE_SIZE >= map->col ||
        (player->coord.y + TILE_SIZE - 1) / TILE_SIZE >= map->row)
        return true;

    /*

        [TODO HACKATHON 2-1] 
    
        Check every corner of enemy if it's collide or not

        We check every corner if it's collide with the wall/hole or not
        You can activate DRAW_HITBOX in utility.h to see how the collision work

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;

    */

    int coordx = player->coord.x / TILE_SIZE;
    int coordy = player->coord.y / TILE_SIZE;
    int coordxmax = (player->coord.x + TILE_SIZE - 1) / TILE_SIZE;
    int coordymax = (player->coord.y + TILE_SIZE - 1) / TILE_SIZE;
    
    if(!isWalkable(map->map[coordy][coordx])) return true;
    if(!isWalkable(map->map[coordy][coordxmax])) return true;
    if(!isWalkable(map->map[coordymax][coordx])) return true;
    if(!isWalkable(map->map[coordymax][coordxmax])) return true;

    return false;
}

extern int token;
void hitPlayer(Player * player, Point enemy_coord, int damage, int thedelay, int KB){
    if(player->knockback_CD == 0){
        float dY = player->coord.y - enemy_coord.y;
        float dX = player->coord.x - enemy_coord.x;
        float angle = atan2(dY, dX);

        /*
            [TODO Homework]
            
            Game Logic when the player get hit or die

            player->knockback_angle = angle;
            player->knockback_CD = 32;

            player->health = ...
            if(player->health <= 0){

                player->health = 0;
                player->status = ...

                ...
            }
        */
        player->knockback_angle = angle;
        
        player->knockback_CD = KB;
        delay = thedelay;
        player->health -= damage;
        remaininghp -= damage;
        
            if (player->health <= 0 && !token) {
                player->health = 0;
                player->status = PLAYER_DYING;
            }
            else if (player->health <= 0 && token) {
                al_play_sample(al_load_sample("Assets/Audio/Pop sound effect.mp3"), SFX_VOLUME, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                player->health = player->maxhealth;
                remaininghp = player->health;
                secondlife = 1;
            }
    }
}
