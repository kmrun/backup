#include "bullet.h"
#include "enemy.h"

#include <math.h>

Bullet create_bullet(char* bullet_path, PointFloat coord, float angle, float speed, float damage){
    Bullet bullet;
    bullet.coord = coord;
    bullet.angle = angle;
    bullet.speed = speed;
    bullet.damage = damage;
    bullet.image = al_load_bitmap(bullet_path);

    // For better repositioning
    bullet.coord.x += (float)(TILE_SIZE/2) * cos(bullet.angle);
    bullet.coord.y += (float)(TILE_SIZE/2) * sin(bullet.angle);
    
    return bullet;
}

// Return true if the bullet collides, so it will be deleted from the list
bool update_bullet(Bullet * bullet, enemyNode * enemyList, Map * map){
    /*
        [TODO Hackathon 3-1] 
        
        Calculate the correct formula to update the bullet,
        Parameter: bullet->speed, bullet->angle

        bullet->coord.x += bullet->speed * cos(bullet->angle);
        bullet->coord.y += bullet->speed * sin(bullet->angle);
    */


    bullet->coord.x += bullet->speed * cos(bullet->angle);
    bullet->coord.y += bullet->speed * sin(bullet->angle);
    
    /*
        [TODO Hackathon 2-3] 
        
        Return true if the bullet collides, so it will deleted from the lists

        int tile_y = (int)(bullet->coord.x / TILE_SIZE);
        int tile_x = (int)(bullet->coord.y / TILE_SIZE);
        if(tile_x < 0 || ...  || map->map[tile_x][tile_y] == ...){
            return true;
        }
    */

    int tile_y = (int)(bullet->coord.y / TILE_SIZE);
    int tile_x = (int)(bullet->coord.x / TILE_SIZE);
    if (tile_x < 0 || tile_y < 0 || tile_y >= map->row || tile_x >= map->col || 
        map->map[tile_y][tile_x] == WALL || 
        map->map[tile_y][tile_x] == DOOR) {
        return true;
    }
    
    // Check if the bullet collide with the enemies by simple iterating
    enemyNode * cur = enemyList->next;
    while(cur != NULL){
        Point enemyCoord = cur->enemy.coord;

        /*
            [TODO Hackathon 3-2] Hit the enemy
            
            Return true if the bullet hit the enemy so we can erase the bullet

            In here, we iterate list of enemies until we hit one of them

            Hint: The enemy coordinate is defined at enemyCoord variable,
            while their width & height are equal to TILE_SIZE

            if(...){
                hitEnemy(...);
                return true;
            }
        */

        int truetile;
        int shift;

        switch (cur->enemy.type) {
            case goblin:
                truetile = (TILE_SIZE * 4)/5;
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
        
        if (cur->enemy.type == bigskelly) {
            if (bullet->coord.x > enemyCoord.x + shift &&
                bullet->coord.y > enemyCoord.y + shift - truetile &&
                bullet->coord.x <= enemyCoord.x + truetile + shift &&
                bullet->coord.y <= enemyCoord.y + truetile + shift &&
                cur->enemy.status == ALIVE) {
                hitEnemy(&cur->enemy, bullet->damage, bullet->angle);
                return true;
            }
        }
        else if (cur->enemy.type == GOBKING) {
            if (bullet->coord.x > enemyCoord.x &&
                bullet->coord.y > enemyCoord.y - truetile + truetile/4 &&
                bullet->coord.x <= enemyCoord.x + truetile - truetile/4 &&
                bullet->coord.y <= enemyCoord.y + truetile/2 &&
                cur->enemy.status == ALIVE) {
                hitEnemy(&cur->enemy, bullet->damage, bullet->angle);
                return true;
            }
        }
        else {
            if (bullet->coord.x > enemyCoord.x + shift &&
                bullet->coord.y > enemyCoord.y + shift &&
                bullet->coord.x <= enemyCoord.x + truetile + shift &&
                bullet->coord.y <= enemyCoord.y + truetile + shift &&
                cur->enemy.status == ALIVE) {
                hitEnemy(&cur->enemy, bullet->damage, bullet->angle);
                return true;
            }
        }

        cur = cur->next;
    }
    return false;
}

void draw_bullet(Bullet * bullet, Point camera){
    float scale = TILE_SIZE / 16;
    //al_draw_filled_circle(bullet->coord.x - camera.x, bullet->coord.y - camera.y, scale, al_map_rgb(255, 255, 0));
    al_draw_bitmap(bullet->image, bullet->coord.x - camera.x - 16, bullet->coord.y - camera.y - 16, 0);
}

void destroy_bullet(Bullet * bullet){
    // In case you are using immage asset for the bullet, free it here
    if (bullet && bullet->image) {
        al_destroy_bitmap(bullet->image);
    }
}

/*
    LINKED LIST IMPLEMENTATION FOR BULLET
    THERE ARE :
        (1) CREATE      : INITIALIZING DUMMY HEAD
        (2) INSERT      : INSERT NEW NODE OF BULLET
        (3) UPDATE      : ITERATE EVERYTHING AND UPDATE THE BULLET
        (4) DRAW        : ITERATE EVERYTHING AND DRAW THE BULLET
        (5) DESTROY     : DESTROY THE LINKED LIST
 */

BulletNode * createBulletList(void){
    BulletNode * dummyhead = (BulletNode *) malloc(sizeof(BulletNode));
    dummyhead->next = NULL;
    return dummyhead;
}

void insertBulletList(BulletNode * dummyhead, Bullet bullet){
    BulletNode * tmp = (BulletNode *) malloc(sizeof(BulletNode));
    tmp->bullet = bullet;
    tmp->next = dummyhead->next;
    dummyhead->next = tmp;
}

void updateBulletList(BulletNode * dummyhead, enemyNode * enemyList, Map * map){
    BulletNode * cur = dummyhead->next;
    BulletNode * prev = dummyhead;
    
    while(cur != NULL){
        bool shouldDelete = update_bullet(&cur->bullet, enemyList, map);
        if(shouldDelete){
            prev->next = cur->next;
            destroy_bullet(&cur->bullet);
            free(cur);
            cur = prev;
        }
        prev = cur;
        cur = cur->next;
    }
}

void drawBulletList(BulletNode * dummyhead, Point camera){
    BulletNode * cur = dummyhead->next;
    
    while(cur != NULL){
        draw_bullet(&cur->bullet, camera);
        cur = cur->next;
    }
}

void destroyBulletList(BulletNode * dummyhead){
    BulletNode* cur = dummyhead->next;
    while(cur != NULL){
        BulletNode * del = cur;
        cur = cur->next;
        destroy_bullet(&del->bullet);
        free(del);
    }
    free(dummyhead);
}
