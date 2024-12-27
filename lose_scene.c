#include <allegro5/allegro.h>
#include "lose_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "game_scene.h"
#include "player.h"

static Button backButton;
static ALLEGRO_BITMAP* lose_bitmap;

static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    lose_bitmap = al_load_bitmap("Assets/panda_lose.png");
}

static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
    }

}

static void draw(void) {
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        150,
        ALLEGRO_ALIGN_CENTER,
        "YOU LOSE"
    );

    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        145,
        ALLEGRO_ALIGN_CENTER,
        "YOU LOSE"
    );
   
    al_draw_scaled_bitmap(lose_bitmap,
        0, 0, 64, 64,
        SCREEN_W / 4, SCREEN_H / 4, 64*6, 64*6, 0);
    
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK TO MENU"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "BACK TO MENU"
    );
}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_lose_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "lose";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
