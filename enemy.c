#define _USE_MATH_DEFINES
#include "enemy.h"
#include "utility.h"
#include "game_scene.h"

#include <math.h>
#include <stdlib.h>

#define QUEUE_SIZE 2000

/*
    ENEMY IMPLEMENTATION
 */

ALLEGRO_BITMAP * slimeBitmap;
ALLEGRO_BITMAP* goblinBitmap;
ALLEGRO_BITMAP* skeleBitmap;
ALLEGRO_BITMAP* gobkingyes;

// To check if p0 sprite and p1 sprite can go directly
static bool validLine(Map* map, Point p0, Point p1);
// To check if we can have a movement cutting directly to it
static bool bresenhamLine(Map * map, Point p0, Point p1);
// Find shortest path using BFS, could be better if you imeplement your own heap
static Point shortestPath(Map * map, Point src, Point dst);
// Calulate the movement speed to update and scaled it
static Point findScaledDistance(Point p1, Point p2);
// Return true if enemy have collision with unwalkable tiles in map
static bool isCollision(Point enemyCoord, Map* map, enemyType type);
// Return true if player collide with enemy
static bool playerCollision(Point enemyCoord, Point playerCoord, enemyType type);

int flagE = 0;

void initEnemy(void){
    // For memory efficiency, we load the image once
    // as every enemy bitmap are pointer, they will point to the same global bitmap variable

    // Slime
    char * slimePath = "Assets/Slime.png";
    slimeBitmap = al_load_bitmap(slimePath);
    if(!slimeBitmap){
        game_abort("Error Load Bitmap with path : %s", slimePath);
    }
    goblinBitmap = al_load_bitmap("Assets/gobgob.png");
    skeleBitmap = al_load_bitmap("Assets/skelebob.png");
    gobkingyes = al_load_bitmap("Assets/gobking.png");
}

Enemy createEnemy(int row, int col, char type){
    game_log("Creating Enemy at ( %d, %d )", row, col);
    
    Enemy enemy;
    memset(&enemy, 0, sizeof(enemy));
    
    enemy.animation_tick = 0;
    enemy.death_animation_tick = 0;
    enemy.status = ALIVE;
    
    enemy.coord = (Point){
        col * TILE_SIZE,
        row * TILE_SIZE
    };
    
    switch(type){
        case 'S':
            enemy.health = 10;
            enemy.type = slime;
            enemy.speed = 3;
            enemy.image = slimeBitmap;
            enemy.kb = 32;
            break;
        // Insert more here to have more enemy variant
        case 'G':
            enemy.health = 6;
            enemy.type = goblin;
            enemy.speed = 4;
            enemy.image = goblinBitmap;
            enemy.kb = 32;
            break;
        case 'K':
            enemy.health = 1;
            enemy.type = bigskelly;
            enemy.speed = 1;
            enemy.image = skeleBitmap;
            enemy.kb = 48;
            break;
        case 'Y':
            enemy.health = 50;
            enemy.type = GOBKING;
            enemy.speed = 0;
            enemy.image = gobkingyes;
            enemy.kb = 48;
            break;
        default:
            enemy.health = 10;
            enemy.type = slime;
            enemy.speed = 3;
            enemy.image = slimeBitmap;
            break;
    }
    
    return enemy;
}

extern int enemycount;
extern int storeprompt;
extern int store;
// Return True if the enemy is dead
bool updateEnemy(Enemy * enemy, Map * map, Player * player){
    flagE = (enemy->dir == RIGHT) ? 1 : 0;
    if(enemy->status == DYING){
        /*
            [TODO Homework]
            
            Configure the death animation tick for dying animation,
            Return true when the enemy is dead
        */ 
        if (enemy->type == slime) {
            enemy->animation_tick = (enemy->death_animation_tick++) % 48;
            if (enemy->death_animation_tick >= 48) {
                enemy->status = DEAD;
                enemycount--;
                return true;
            }
        } else if (enemy->type == goblin) {
            enemy->animation_tick = (enemy->death_animation_tick++) % 72;
            if (enemy->death_animation_tick >= 72) {
                enemy->status = DEAD;
                enemycount--;
                return true;
            }
        }
        else if (enemy->type == bigskelly) {
            enemy->animation_tick = (enemy->death_animation_tick++) % 150;
            if (enemy->death_animation_tick >= 150) {
                enemy->status = DEAD;
                enemycount--;
                return true;
            }
        }
    }
    
    if(enemy->status != ALIVE) return false;

    if(enemy->animation_hit_tick > 0){
        if (enemy->type == slime) {
            enemy->animation_tick = (enemy->animation_tick + 1) % 64;
            enemy->animation_hit_tick--;
        } else if (enemy->type == goblin) {
            enemy->animation_tick = (enemy->animation_tick + 1) % 72;
            enemy->animation_hit_tick--;
        }
        else if (enemy->type == bigskelly) {
            enemy->animation_tick = (enemy->animation_tick + 1) % 90;
            enemy->animation_hit_tick--;
        }
    }
    else {
        if (enemy->type == bigskelly) {
            enemy->animation_tick = (enemy->animation_tick + 1) % 130;
        }
        else {
            enemy->animation_tick = (enemy->animation_tick + 1) % 64;
        }
    }
    
    if(enemy->knockback_CD > 0){
        enemy->knockback_CD--;
        int next_x = enemy->coord.x + 4  * cos(enemy->knockback_angle);
        int next_y = enemy->coord.y + 4 * sin(enemy->knockback_angle);
        Point next;
        next = (Point){next_x, enemy->coord.y};
        
        if(!isCollision(next, map, enemy->type)){
            enemy->coord = next;
        }
        
        next = (Point){enemy->coord.x, next_y};
        if(!isCollision(next, map, enemy->type)){
            enemy->coord = next;
        }
    }
    else{
        /*
            [TODO Homework]
            
            Replace delta variable with the function below to start enemy movement
            Point delta = shortestPath(map, enemy->coord, player->coord);
        */
        Point delta = (Point){ 0,0 };
        /*if (enemy->animation_hit_tick = 0) {
            Point delta = shortestPath(map, enemy->coord, player->coord);
        }*/
        Point next, prev = enemy->coord;
        
        if (delta.x > 0) enemy->dir = RIGHT;
        if (delta.x < 0) enemy->dir = LEFT;        

        next = (Point){enemy->coord.x + delta.x * enemy->speed, enemy->coord.y};
        if(!isCollision(next, map, enemy->type))
            enemy->coord = next;
        
        next = (Point){enemy->coord.x, enemy->coord.y + delta.y * enemy->speed};
        if(!isCollision(next, map, enemy->type))
            enemy->coord = next;
        
        // To fix bug if the enemy need to move a little bit, the speed will not be use
        if(enemy->coord.x == prev.x && enemy->coord.y == prev.y){
            next = (Point){enemy->coord.x + delta.x, enemy->coord.y};
            if(!isCollision(next, map, enemy->type))
                enemy->coord = next;
            
            next =(Point){enemy->coord.x, enemy->coord.y + delta.y};
            if(!isCollision(next, map, enemy->type))
                enemy->coord = next;
        }
        
        if (playerCollision(enemy->coord, player->coord, enemy->type) && enemy->animation_hit_tick == 0) {
            if (enemy->type == slime) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 56;
                hitPlayer(player, enemy->coord, 1, 0, 32);
            }
            else if (enemy->type == goblin) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 26;
                hitPlayer(player, enemy->coord, 1, 0, 32);
            }
            else if (enemy->type == bigskelly) {
                enemy->animation_tick = 0;
                enemy->animation_hit_tick = 40;
                hitPlayer(player, enemy->coord, 2, 16, 48);
            }
        }

        /*for store prompt*/
        if (playerCollision(enemy->coord, player->coord, enemy->type) && enemy->type == GOBKING) {
            storeprompt = 1;
        }
        else if (!playerCollision(enemy->coord, player->coord, enemy->type) && enemy->type == GOBKING) {
            storeprompt = 0;
            store = 0;
        }

    }

    return false;
}

extern int gobkingsurprise;
void drawEnemy(Enemy * enemy, Point cam){
    int dy = enemy->coord.y - cam.y; // destiny y axis
    int dx = enemy->coord.x - cam.x; // destiny x axis

    //based on enemy type
    int offsetx = 0;
    int offsety = 0;
    if (enemy->type == slime) {
        offsetx = 16 * (int)(enemy->animation_tick / 8);
    }
    else if (enemy->type == goblin) {
        offsetx = 16 + 32 * (enemy->animation_tick / 8) + 64 * (enemy->animation_tick / 8);
        offsety = 128;
    }
    else if (enemy->type == bigskelly) {
        offsetx = 22 * (enemy->animation_tick / 10);
        
    }
    
    int sussyskelly = (!flagE) ? 9 : 0;
    ALLEGRO_COLOR slimespecial = enemy->knockback_CD > 0 ? al_map_rgba(80, 0, 0, 150) : al_map_rgb(0, 255, 255);

    if(enemy->status == ALIVE){
        if(enemy->animation_hit_tick > 0){
            if (enemy->type == slime) {
                offsetx = 16 * 8 + 16*(enemy->animation_tick/8);
            } else if (enemy->type == goblin) {
                offsetx = 16 + 32 * (enemy->animation_tick / 3) + 64 * (enemy->animation_tick / 3);
                offsety = 0;
            }
            else if (enemy->type == bigskelly) {
                offsetx = 43 * (enemy->animation_tick / 3);
                offsety = 97;
                sussyskelly += (flagE) ? 4: 16;
            }
        }

        ALLEGRO_COLOR hitmark = enemy->knockback_CD > 0 ? al_map_rgba(255, 0, 0, 150) : al_map_rgb(255,255,255);
        
        if (enemy->type == slime) {
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 16, 16, slimespecial,
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flagE);
        } 
        else if (enemy->type == goblin) {
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 64, 64, hitmark,
                21.5f, 20, dx, dy, TILE_SIZE / 20, TILE_SIZE / 20,
                0, !flagE);
        }
        else if (enemy->type == bigskelly) {
            if (enemy->animation_hit_tick > 0) {
                al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 43, 37, hitmark,
                    sussyskelly, 24, dx, dy, TILE_SIZE / 11, TILE_SIZE / 11,
                    0, !flagE);
            }
            else {
                al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 22, 33, hitmark,
                    sussyskelly, 21, dx, dy, TILE_SIZE / 11, TILE_SIZE / 11,
                    0, !flagE);
            }
        }
        else if (enemy->type == GOBKING) {
            if (gobkingsurprise) {
                offsety = 128;
                offsetx = 64 * (enemy->animation_tick / 8);
                if (enemy->animation_tick == 63) {
                    gobkingsurprise = 0;
                }
            }
            else {
                offsetx = 64 * (enemy->animation_tick / 16);
            }

            al_draw_tinted_scaled_rotated_bitmap_region(gobkingyes,
                offsetx, offsety, 64, 64, al_map_rgb(255, 255, 255),
                16, 48, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16, 0, 0);
        }
    }
    else if(enemy->status == DYING){
        /*
            [TODO Homework]

            Draw Dying Animation for enemy
        */
        if (enemy->type == slime) {
            offsetx = 16 * (enemy->animation_tick / 6);
            offsety = 16;
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 16, 16, al_map_rgb(255,255,255),
                0, 0, dx, dy, TILE_SIZE / 16, TILE_SIZE / 16,
                0, flagE);
        } 
        else if (enemy->type == goblin) {
            offsetx = 16 + 32 * (enemy->animation_tick / 8) + 64 * (enemy->animation_tick / 8);
            offsety = 64 * 3;
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 64, 64, al_map_rgb(255, 255, 255),
                21.5f, 20, dx, dy, TILE_SIZE / 20, TILE_SIZE / 20,
                0, !flagE);
        }
        else if (enemy->type == bigskelly) {
            offsetx = 33 * (enemy->animation_tick / 10);
            offsety = 65;
            sussyskelly += (flagE) ? 12 : 0;
            al_draw_tinted_scaled_rotated_bitmap_region(enemy->image, offsetx, offsety, 33, 32, al_map_rgb(255, 255, 255),
                sussyskelly, 21, dx, dy, TILE_SIZE / 11, TILE_SIZE / 11,
                0, !flagE);
        }
    }
    
//#ifdef DRAW_HITBOX
//    int truetile;
//    int shift;
//
//    switch (enemy->type) {
//        case goblin:
//            truetile = (TILE_SIZE*4) / 5;
//            shift = (TILE_SIZE - truetile) / 2;
//            break;
//        case GOBKING:
//            truetile = (TILE_SIZE) * 2;
//            shift = (TILE_SIZE - truetile) / 2;
//            break;
//        default:
//            truetile = TILE_SIZE;
//            shift = 0;
//            break;
//    }
//    //for non square monsters
//    if (enemy->type == bigskelly) {
//        al_draw_rectangle(
//            dx + shift - (truetile / 2)*!flagE, dy + shift - truetile, dx + truetile + shift + (truetile / 2)*flagE, dy + truetile + shift,
//            al_map_rgb(255, 0, 0), 1
//        );
//    }
//    else if (enemy->type == GOBKING) {
//        al_draw_rectangle(
//            dx, dy - truetile, dx + (truetile*3)/2, dy + truetile/2,
//            al_map_rgb(255, 0, 0), 1
//        );
//    }
//    //for square monsters
//    else {
//        al_draw_rectangle(
//            dx + shift, dy + shift, dx + truetile + shift, dy + truetile + shift,
//            al_map_rgb(255, 0, 0), 1
//        );
//    }
//#endif
}

void destroyEnemy(Enemy * enemy){

}

void terminateEnemy(void) {
    al_destroy_bitmap(slimeBitmap);
}

void hitEnemy(Enemy * enemy, int damage, float angle){

    /*  
        [TODO Homework]

        Decrease the enemy health with damage, if the health < 0, then set the status to DYING
    
        enemy->health = ...
        if(...){
            enemy->status = DYING;
        }
    */

    enemy->health -= damage;
        if (enemy->health <= 0) {
            enemy->status = DYING;
        }

    enemy->knockback_angle = angle;
    if (enemy->type == GOBKING) {
        enemy->knockback_CD = 0;
    }
    else {
        enemy->knockback_CD = 16;
    }
}

/*
    LINKED LIST IMPLEMENTATION FOR ENEMY
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF ENEMY
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE ENEMY
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE ENEMY
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

enemyNode * createEnemyList(void){
    enemyNode * dummyhead = (enemyNode *) malloc(sizeof(enemyNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertEnemyList(enemyNode * dummyhead, Enemy _enemy){
    enemyNode * tmp = (enemyNode *) malloc(sizeof(enemyNode));
    tmp->enemy = _enemy;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

void updateEnemyList(enemyNode * dummyhead, Map * map, Player * player){
    enemyNode * cur = dummyhead->next;
    enemyNode * prev = dummyhead;
    
    while(cur != NULL){
        bool shouldDelete = updateEnemy(&cur->enemy, map, player);
        if(shouldDelete){
            prev->next = cur->next;
            destroyEnemy(&cur->enemy);
            free(cur);
            cur = prev->next;
        }
        else{
            prev = cur;
            cur = cur->next;
        }
    }
}

void drawEnemyList(enemyNode * dummyhead, Point cam){
    enemyNode * cur = dummyhead->next;
    
    while(cur != NULL){
        drawEnemy(&cur->enemy, cam);
        cur = cur->next;
    }
}

void destroyEnemyList(enemyNode * dummyhead){
    while(dummyhead != NULL){
        enemyNode * del = dummyhead;
        dummyhead = dummyhead->next;
        destroyEnemy(&del->enemy);
        free(del);
    }
}

static bool validLine(Map * map, Point p0, Point p1){
    int offsetX[4] = {0, 0, TILE_SIZE-1, TILE_SIZE-1};
    int offsetY[4] = {0, TILE_SIZE-1, 0, TILE_SIZE-1};
    
    for(int i=0; i<4; i++){
        Point n1 = (Point){
            p0.x + offsetX[i],
            p0.y + offsetY[i]
        };
        
        Point n2 = (Point){
            p1.x + offsetX[i],
            p1.y + offsetY[i]
        };
        if(bresenhamLine(map, n1, n2)) return false;
    }
    return true;
}

static Point shortestPath(Map * map, Point enemy, Point player){
    // Point enemy & player is pixel coordinate
    static DIRECTION dir[MAX_MAP_ROW][MAX_MAP_COL]; // to backtrack from dst to src
    static bool visit[MAX_MAP_ROW][MAX_MAP_COL];
    memset(visit, 0, sizeof(visit));

    // Point declared below is not coordinate of pixel, but COORDINATE OF ARRAY!
    Point src = (Point){
        enemy.y / TILE_SIZE,
        enemy.x / TILE_SIZE
    };

    Point dst = (Point){
        player.y / TILE_SIZE,
        player.x / TILE_SIZE
    };
    
    static Point Queue[QUEUE_SIZE];
    int front = 0, rear = 0;
    
    Queue[rear++] = src;
    bool found = false;
    
    // Movement set
    static int dx[4] = {1, -1, 0, 0};
    static int dy[4] = {0, 0, 1, -1};
    static DIRECTION move[4] = {UP, DOWN, LEFT, RIGHT}; // To backtrack
    
    while(front != rear){
        Point cur = Queue[front++];
        
        // Found the destiny
        if(cur.x == dst.x && cur.y == dst.y) {
            found = true;
            break;
        };
        
        for(int i=0; i<4; i++){
            Point next = (Point){
                cur.x + dx[i],
                cur.y + dy[i]
            };
            
            if(next.x < 0 || next.y < 0) continue;
            if(next.x >= map->row || next.y >= map->col) continue;
            
            if(isWalkable(map->map[next.x][next.y]) && !visit[next.x][next.y]){
                dir[next.x][next.y] = move[i];
                visit[next.x][next.y] = true;
                Queue[rear++] = next;
            }
        }
    }
    
    // Toward a String-Pulling Approach to Path Smoothing on Grid Graphs
    // http://idm-lab.org/bib/abstracts/papers/socs20c.pdf
    if(found){
        if(validLine(map, enemy, player))
            return findScaledDistance(enemy, player);
        
        int max_iteration = 1000;
        Point it = dst;

        
        while(max_iteration--){
            Point translate_it = (Point){
                it.y * TILE_SIZE,
                it.x * TILE_SIZE
            };
            
            if(validLine(map, enemy, translate_it))
                return findScaledDistance(enemy, translate_it);
            
            switch(dir[it.x][it.y]){
                case UP:
                    it.x--;
                    break;
                case DOWN:
                    it.x++;
                    break;
                case LEFT:
                    it.y--;
                    break;
                case RIGHT:
                    it.y++;
                    break;
                default:
                    goto END;
            }
        }
    END:
        game_log("failed to iterate");
    }
    return (Point){0, 0};
} 

static bool bresenhamLine(Map * map, Point p0, Point p1){
    int dx =  abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy =  -abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        int tile_x = p0.y / TILE_SIZE;
        int tile_y = p0.x / TILE_SIZE;
        if(!isWalkable(map->map[tile_x][tile_y])) return true;
        
        if (p0.x == p1.x && p0.y == p1.y) break;
        e2 = 2 * err;

        if (e2 > dy) {
            err += dy;
            p0.x += sx;
        } else if (e2 < dx) {
            err += dx;
            p0.y += sy;
        }
    }
    
    return false;
}

static Point findScaledDistance(Point p1, Point p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    
    double d = sqrt(dx * dx + dy * dy);
    
    // Floating error fix, when smaller than delta it will be immediately 0
    if (d < 0.001) {
        return (Point) {0, 0};
    }

    double dxUnit = dx / d;
    double dyUnit = dy / d;
    
    return (Point){round(dxUnit), round(dyUnit)};
}


static bool playerCollision(Point enemyCoord, Point playerCoord, enemyType type){
    int truetile;
    int shift;

    switch (type) {
        case goblin:
            truetile = (TILE_SIZE * 4) / 5;
            shift = (TILE_SIZE - truetile) / 2;
            break;
        case bigskelly:
            truetile = (TILE_SIZE);
            shift = (TILE_SIZE - truetile) / 2;
            break;
        case GOBKING:
            truetile = (TILE_SIZE) * 2;
            shift = (TILE_SIZE - truetile) / 2;
            break;
        default:
            truetile = TILE_SIZE;
            shift = 0;
            break;
    }

    // Rectangle & Rectangle Collision
    if (type == bigskelly) {
        if (enemyCoord.x + shift - (truetile/2)*!flagE < playerCoord.x + TILE_SIZE &&
            enemyCoord.x + truetile + shift + (truetile/2)*flagE > playerCoord.x &&
            enemyCoord.y + shift - truetile < playerCoord.y + TILE_SIZE &&
            enemyCoord.y + truetile + shift > playerCoord.y) {
            return true;
        }
        else {
            return false;
        }
    }
    else if (type == GOBKING) {
        if (enemyCoord.x < playerCoord.x + TILE_SIZE &&
            enemyCoord.x + (truetile * 3) / 2 > playerCoord.x &&
            enemyCoord.y - truetile < playerCoord.y + TILE_SIZE &&
            enemyCoord.y + truetile / 2 > playerCoord.y) {
            return true;
        }
        else {
            return false;
        }
    }
    /*dx, dy - (truetile * 3) / 2, dx + (truetile * 3) / 2, dy + truetile / 2,*/
    else {
        if (enemyCoord.x + shift < playerCoord.x + TILE_SIZE &&
            enemyCoord.x + truetile + shift > playerCoord.x &&
            enemyCoord.y + shift < playerCoord.y + TILE_SIZE &&
            enemyCoord.y + truetile + shift > playerCoord.y) {
            return true;
        }
        else {
            return false;
        }
    }
}
    
static bool isCollision(Point enemyCoord, Map* map, enemyType type){
    if (enemyCoord.x < 0 ||
        enemyCoord.y < 0 ||
        (enemyCoord.x + TILE_SIZE) / TILE_SIZE >= map->col ||
        (enemyCoord.y + TILE_SIZE) / TILE_SIZE >= map->row)
        return true;
    /* 
        [TODO HACKATHON 2-2] 

        Check every corner of enemy if it's collide or not
        Return true if it's not walkable

        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
        if(!isWalkable(map->map[...][...])) return true;
    */

    int coordx, coordy, coordxmax, coordymax;
    /*if (type == bigskelly) {
        coordx = (enemyCoord.x + shift) / (TILE_SIZE);
        coordy = (enemyCoord.y + shift - truetile/2) / (TILE_SIZE);
        coordxmax = (enemyCoord.x + truetile + shift - 1) / (TILE_SIZE);
        coordymax = (enemyCoord.y + truetile + shift + truetile/2 - 1) / (TILE_SIZE);
    }
    else {*/
        coordx = (enemyCoord.x) / (TILE_SIZE);
        coordy = (enemyCoord.y)/ (TILE_SIZE);
        coordxmax = (enemyCoord.x + TILE_SIZE - 1) / (TILE_SIZE);
        coordymax = (enemyCoord.y + TILE_SIZE - 1) / (TILE_SIZE);
    //}

    if (!isWalkable(map->map[coordy][coordx])) return true;
    if (!isWalkable(map->map[coordy][coordxmax])) return true;
    if (!isWalkable(map->map[coordymax][coordx])) return true;
    if (!isWalkable(map->map[coordymax][coordxmax])) return true;

    return false;
}
