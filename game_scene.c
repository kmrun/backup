#include <string.h>
#include "game_scene.h"
#include "menu_scene.h"
#include "loading_scene.h"
#include "game.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "weapon.h"
#include "UI.h"
#include "lose_scene.h"
#include "win_scene.h"


#include <math.h>
#include <stdbool.h>

Player player; // Player
Player player2;
Map map; // Map
enemyNode * enemyList; // Enemy List
BulletNode * bulletList; // Bullet List
static ALLEGRO_BITMAP* heart_bitmap;
static ALLEGRO_BITMAP* greyheart_bitmap;
static ALLEGRO_BITMAP* coin_bitmap;
static ALLEGRO_BITMAP* woodui;
static Button purchase;
static Button equip;
// Weapons
Weapon weapon; 
Weapon sniper;

// store img
static ALLEGRO_BITMAP* STOREIMG;

int startwithsix = 0;
int coins_obtained = 0;
int lantai = 1;
int pause = 0;
int nextlevelprompt = 0;
int storeprompt = 0;
int store = 0;
int enemycount = 0;
int KILLTHEMALL = 0;
int CDthingy = 0;
int storeloop = 0;
int storecounter = 0;
int remaininghp = 5;
int token = 0;
int secondlife = 0;

int gctick = 0;
int gctick2 = 0;

static void init(void){
    pause = 0;
    initEnemy();

    switch (lantai) {
        case 1:
            map = create_map("Assets/map0.txt", 0);
            break;
        case 2:
            map = create_map("Assets/map1.txt", 0);
            break;
        case 3:
            map = create_map("Assets/map2.txt", 0);
            break;
        default:
            break;
    }

    player = create_player("Assets/panda2.png", map.Spawn.x, map.Spawn.y);

    enemyList = createEnemyList();
    bulletList = createBulletList();

    weapon = create_weapon("Assets/guns.png", "Assets/yellow_bullet.png", 24, 18, 1);
    sniper = create_weapon("Assets/sniper.png", "Assets/yellow_bullet.png", 96, 36, 5);

    woodui = al_load_bitmap("Assets/woodui.png");

    //hearts bitmap
    heart_bitmap = al_load_bitmap("Assets/heart.png");
    greyheart_bitmap = al_load_bitmap("Assets/greyheart.png");

    //coin bitmap
    coin_bitmap = al_load_bitmap("Assets/coin_icon.png");

    //store assets
    purchase = button_create(SCREEN_W / 2 - 100, 675, 350, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered2.png");
    equip = button_create(SCREEN_W / 2 - 100, 550, 350, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered2.png");

    for(int i=0; i<map.EnemySpawnSize; i++){
        Enemy enemy = createEnemy(map.EnemySpawn[i].x, map.EnemySpawn[i].y, map.EnemyCode[i]);
        insertEnemyList(enemyList, enemy);
    }

    game_log("coord x:%d \n coords y:%d \n", map.Spawn.x, map.Spawn.y);
    change_bgm("Assets/audio/game_bgm.mp3");
}

int bought1 = 1, bought2 = 0, bought3 = 0, bought4 = 0, bought5 = 0;
int price1 = 0, price2 = 0, price3 = 0, price4 = 0, price5 = 0;
int weapon1 = 1, weapon2 = 0;
static void update(void){
    /*
        [TODO Homework]
        
        Change the scene if winning/losing to win/lose scene
    */

    gctick = (gctick + 1) % 96;
    gctick2 = (gctick2 + 1) % 96;

    if (player.status == PLAYER_DEAD) {
        lantai = 1;
        change_scene(create_lose_scene());
        enemycount = 0;
        coins_obtained = 0;
        return;
    }

    if (nextlevelprompt) {
        if (keyState[ALLEGRO_KEY_ESCAPE]) {
            player.coord.x = player.coord.x - TILE_SIZE;
            player.coord.y = player.coord.y + TILE_SIZE;
            nextlevelprompt = 0;
            pause = 0;
            al_rest(0.2);
            return;
        }
        if (keyState[ALLEGRO_KEY_ENTER]) {
            if (enemycount) {
                player.coord.x = player.coord.x - TILE_SIZE;
                player.coord.y = player.coord.y + TILE_SIZE;
                nextlevelprompt = 0;
                pause = 0;
                al_rest(0.2);
                KILLTHEMALL = 180;
            }
            else if (!enemycount){
                lantai++;
                change_scene(create_loading_scene());
                enemycount = 0;
                nextlevelprompt = 0;
            }
            return;
        }
    }

    if (keyState[ALLEGRO_KEY_ESCAPE] && !pause) {
        pause = 1;
        al_rest(0.2);
    }
    else if (keyState[ALLEGRO_KEY_ESCAPE] && pause) {
        pause = 0;
        al_rest(0.2);
    }
    else if (keyState[ALLEGRO_KEY_ENTER] && pause) {
        change_scene(create_menu_scene());
        al_rest(0.2);
        enemycount = 0;
        coins_obtained = 0;
        lantai = 1;
    }

    if (pause) {
        return;
    }

    if (player.status == WIN) {
        change_scene(create_win_scene());
        enemycount = 0;
    }

    update_player(&player, &map);


    Point Camera;
    /*
        [TODO HACKATHON 1-3]
        
        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Adjust it based on player position variable, then subtract it with half of the gameplay screen
    */
    Camera.x = player.coord.x - SCREEN_W / 2 + TILE_SIZE / 2;
    Camera.y = player.coord.y - SCREEN_H / 2 + TILE_SIZE / 2;

    if (player.status == PLAYER_DYING) {
        return;
    }

    updateEnemyList(enemyList, &map, &player);
    if (weapon1) {
        update_weapon(&weapon, bulletList, player.coord, Camera);
    }
    else {
        update_weapon(&sniper, bulletList, player.coord, Camera);
    }
    updateBulletList(bulletList, enemyList, &map);
    update_map(&map, player.coord, &coins_obtained);

    if (storeprompt && !store && keyState[ALLEGRO_KEY_E] && !CDthingy) {
        store = 1;
        CDthingy = 24;
        storeloop = 0;
        gctick = 0;
    }
    
    if (storeprompt && store && keyState[ALLEGRO_KEY_E] && !CDthingy) {
        store = 0;
        CDthingy = 24;
    }

    if (store) {
        update_button(&purchase);
        update_button(&equip);
        if (keyState[ALLEGRO_KEY_Z] && !CDthingy) {
            CDthingy = 24;
            if (!storecounter) {
                storecounter = 4;
            }
            else {
                storecounter = (storecounter - 1) % 5;
            }
        }
        if (keyState[ALLEGRO_KEY_C] && !CDthingy) {
            CDthingy = 24;
            storecounter = (storecounter + 1) % 5;
        }
        int price;
        int* boughtpointer;
        switch (storecounter) {
            case 0:
                price = price1;
                boughtpointer = &bought1;
                break;
            case 1:
                price = price2;
                boughtpointer = &bought2;
                break;
            case 2:
                price = price3;
                boughtpointer = &bought3;
                break;
            case 3:
                price = price4;
                boughtpointer = &bought4;
                break;
            case 4:
                price = price5;
                boughtpointer = &bought5;
                break;
            default:
                price = price1;
                boughtpointer = &bought1;
                break;
        } 
        if (mouseState.buttons && purchase.hovered == true && coins_obtained >= price && !(*boughtpointer)) {
            *boughtpointer = 1;
            coins_obtained -= price;
            switch (storecounter) {
            case 2: //heart
                player.maxhealth += 1;
                remaininghp += 1;
                player.health += 1;
                startwithsix = 1;
                break;
            case 3: //speed
                player.speed += 1;
                break;
            case 4: // extra life
                token = 1;
                break;
            }
            al_rest(0.1);
        }
        if (mouseState.buttons && equip.hovered == true && (*boughtpointer)) {
            switch (storecounter) {
                case 0:
                    weapon1 = 1;
                    weapon2 = 0;
                    break;
                case 1:
                    weapon1 = 0;
                    weapon2 = 1;
                    break;
                default:
                    break;
            }
            al_rest(0.1);
        }
    }
    if (CDthingy) {
        CDthingy--;
    }
}

extern ALLEGRO_BITMAP* gobkingyes;
static void draw(void) {
    Point Camera;
    /*
        [TODO HACKATHON 1-4]

        Calcualte the formula for the Camera
        Camera.x = ...
        Camera.y = ...

        Hint: Just copy from the [TODO HACKATHON 1-3]
    */
    Camera.x = player.coord.x - SCREEN_W / 2 + TILE_SIZE / 2;
    Camera.y = player.coord.y - SCREEN_H / 2 + TILE_SIZE / 2;

    al_draw_filled_rounded_rectangle(100, 650, 700, 800, 2, 2, al_map_rgb(199, 207, 221));
    // Draw
    draw_map(&map, Camera);
    drawEnemyList(enemyList, Camera);
    drawBulletList(bulletList, Camera);
    draw_player(&player, Camera);

    if (player.status != PLAYER_DYING) {
        if (weapon1) {
            draw_weapon(&weapon, player.coord, Camera);
        }
        else {
            draw_weapon(&sniper, player.coord, Camera);
        }
    }

    if (storeprompt && !store) {
        al_draw_text(P1_FONT, al_map_rgb(255, 255, 255),
            20, 740, 0,
            "\'E\' to interact");
    }

    if (store) {
        int offsety = 192;
        int offsetx;
        if (!storeloop) {
            offsetx = 64 * (gctick / 8);
            if (gctick == 95) {
                storeloop = 1;
                gctick2 = 0;
            }
        }
        else {
            offsetx = 256 + 64 * (gctick2 / 12);
        }

        al_draw_scaled_bitmap(al_load_bitmap("Assets/Carved_9Slides.png"),
            0, 0, 192, 192, 28, 100, 750, 800, 0);
        al_draw_scaled_bitmap(al_load_bitmap("Assets/Ribbon_Red_3Slides.png"),
            0, 0, 192, 64, 0, 70, 800, 140, 0);
        al_draw_text(P1_FONT, al_map_rgb(255, 255, 255),
            SCREEN_W/2, 100, ALLEGRO_ALIGN_CENTER,
            "GOB'S WARES");
        al_draw_tinted_scaled_rotated_bitmap_region(gobkingyes,
            offsetx, offsety, 64, 64, al_map_rgb(255, 255, 255),
            16, 48, 80, 700, TILE_SIZE / 16, TILE_SIZE / 16, 0, 0);
        al_draw_scaled_bitmap(al_load_bitmap("Assets/arrowleft.png"),
            0, 0, 16, 16, 40, 325, 150, 150, 0);
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), 110, 450, ALLEGRO_ALIGN_CENTER, "\'Z\'");
        al_draw_scaled_bitmap(al_load_bitmap("Assets/arrowright.png"),
            0, 0, 16, 16, 620, 325, 150, 150, 0);
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), 695, 450, ALLEGRO_ALIGN_CENTER, "\'C\'");
        char* name;
        int status;
        int price;
        char* thetextofbutton;
        switch (storecounter) {
            case 0:
                STOREIMG = al_load_bitmap("Assets/guns.png");
                name = "pew pew pew";
                status = bought1;
                price = price1;
                break;
            case 1:
                STOREIMG = al_load_bitmap("Assets/sniper.png");
                name = "PEW";
                status = bought2;
                price = price2;
                break;
            case 2:
                STOREIMG = al_load_bitmap("Assets/bonushp.png");
                name = "healthy";
                status = bought3;
                price = price3;
                break;
            case 3:
                STOREIMG = al_load_bitmap("Assets/bootsofspeed.png");
                name = "speedyboots";
                status = bought4;
                price = price4;
                break;
            case 4:
                STOREIMG = al_load_bitmap("Assets/moaiyes.png");
                name = "rock mmm";
                status = bought5;
                price = price5;
                break;
            default:
                STOREIMG = al_load_bitmap("Assets/guns.png");
                name = "pew pew pew";
                status = bought1;
                price = price1;
                break;
        }

        if (storecounter == 0) {
            if (weapon1) {
                thetextofbutton = "EQUIPPED";
            }
            else {
                thetextofbutton = "EQUIP";
            }
            draw_button(equip);
            al_draw_textf(
                P2_FONT,
                al_map_rgb(66, 76, 110),
                SCREEN_W / 2 + 75,
                550 + 28 + equip.hovered * 15,
                ALLEGRO_ALIGN_CENTER,
                "%s", thetextofbutton
            );
            al_draw_textf(
                P2_FONT,
                al_map_rgb(225, 225, 225),
                SCREEN_W / 2 + 75,
                550 + 31 + equip.hovered * 15,
                ALLEGRO_ALIGN_CENTER,
                "%s", thetextofbutton
            );
        }
        if (storecounter == 1) {
            if (weapon2) {
                thetextofbutton = "EQUIPPED";
            }
            else {
                thetextofbutton = "EQUIP";
            }
            draw_button(equip);
            al_draw_textf(
                P2_FONT,
                al_map_rgb(66, 76, 110),
                SCREEN_W / 2 + 75,
                550 + 28 + equip.hovered * 15,
                ALLEGRO_ALIGN_CENTER,
                "%s", thetextofbutton
            );
            al_draw_textf(
                P2_FONT,
                al_map_rgb(225, 225, 225),
                SCREEN_W / 2 + 75,
                550 + 31 + equip.hovered * 15,
                ALLEGRO_ALIGN_CENTER,
                "%s", thetextofbutton
            );

        }

        if (!status) {
            al_draw_textf(P3_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 4 - 50, SCREEN_H / 2 + 165, ALLEGRO_ALIGN_CENTER, " That'll be %d gold", price);
        }
        al_draw_scaled_bitmap(STOREIMG,
            0, 0, 32, 32, SCREEN_W/2 - 150, SCREEN_H/2 - 130, 300, 300, 0);
        al_draw_textf(P2_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 160, ALLEGRO_ALIGN_CENTER, "%s", name);

        draw_button(purchase);
        char* buttontext;
        if (status) {
            buttontext = "OWNED";
        }
        else {
            buttontext = "PURCHASE";
        }
        al_draw_textf(
            P2_FONT,
            al_map_rgb(66, 76, 110),
            SCREEN_W / 2 + 75,
            675 + 28 + purchase.hovered * 15,
            ALLEGRO_ALIGN_CENTER,
            "%s", buttontext
        );
        al_draw_textf(
            P2_FONT,
            al_map_rgb(225, 225, 225),
            SCREEN_W / 2 + 75,
            675 + 31 + purchase.hovered * 15,
            ALLEGRO_ALIGN_CENTER,
            "%s", buttontext
        );
    }

    if (pause && !nextlevelprompt) {
        al_draw_filled_rectangle(0, 0, 800, 800, al_map_rgba(0,0,0,150));
        //add more pause aesthetics
        al_draw_text(P1_FONT, al_map_rgb(255, 255, 255),
            20, 680, 0,
            "ESC (to continue)");
        al_draw_text(P1_FONT, al_map_rgb(255, 255, 255),
            20, 740, 0,
            "ENTER (back to menu)");
    }
    if (nextlevelprompt) {
        al_draw_filled_rectangle(0, 0, 800, 800, al_map_rgba(0, 0, 0, 150));
        al_draw_scaled_bitmap(woodui,
            0, 0, 380, 200,
            80, 500, 620, 250, 0);
        al_draw_text(P1_FONT, al_map_rgb(255,255,255),
            150, 550, 0,
            "Continue..?");
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            150, 650, 0,
            "NO! (ESC)");
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255),
            390, 650, 0,
            "YES (ENTER)");
    }
    if (KILLTHEMALL) {
        al_draw_text(SUPER_FONT, al_map_rgb(255, 0, 0),
            425, 150, ALLEGRO_ALIGN_CENTER,
            "KILL");
        al_draw_text(SUPER_FONT, al_map_rgb(255, 0, 0),
            425, 350, ALLEGRO_ALIGN_CENTER,
            "THEM");
        al_draw_text(SUPER_FONT, al_map_rgb(255, 0, 0),
            425, 550, ALLEGRO_ALIGN_CENTER,
            "ALL");
        KILLTHEMALL--;
    }

    /*
        [TODO Homework]
        
        Draw the UI of Health and Total Coins
    */

    for (int i = 0; i < player.maxhealth; i++) {
        if (i < player.health) {
            al_draw_scaled_bitmap(heart_bitmap,
                0, 0, 32, 32,  
                i * 56, 0,      
                32 * 2.5, 32 * 2.5, 0);
        }
        else {
            // grey heart shit
            al_draw_scaled_bitmap(greyheart_bitmap,
                0, 0, 32, 32,
                i * 56, 0,
                32 * 2.5, 32 * 2.5, 0); 
        }
    }

    al_draw_scaled_bitmap(coin_bitmap,
        0, 0, 16, 16,
        8, 32*2.5-14,
        16 * 4.2, 16 * 4.2, 0);

    al_draw_textf(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        95,
        84,
        ALLEGRO_ALIGN_CENTER,
        "%02d", coins_obtained
    );
}

static void destroy(void){
    delete_player(&player);
    delete_weapon(&weapon);
    destroy_map(&map);
    destroyBulletList(bulletList);
    destroyEnemyList(enemyList);
    terminateEnemy();
    destroy_button(&purchase);
}


Scene create_game_scene(void){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    
    scene.name = "game";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;
    
    return scene;
}
