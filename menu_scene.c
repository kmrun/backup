#include <allegro5/allegro.h>
#include <string.h>
#include "menu_scene.h"
#include "game_scene.h"
#include "setting_scene.h"
#include "loading_scene.h"
#include "leaderboard.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button start;
static Button settingButton;
static Button leaderboard;


static void init(void) {
    start = button_create(SCREEN_W / 2 - 200, 435, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    settingButton = button_create(SCREEN_W / 2 - 200, 540, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    leaderboard = button_create(SCREEN_W / 2 - 200, 645, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    change_bgm("Assets/audio/menu_bgm.mp3");
}

static void update(void) {
    update_button(&settingButton);
    update_button(&start);
    update_button(&leaderboard);
    /*
        [TODO HACKATHON 4-3]

        Change scene to setting scene when the button is pressed
    */
    if (mouseState.buttons && start.hovered == true) {
        change_scene(create_loading_scene());
        al_rest(0.2);
    }
    if (mouseState.buttons && settingButton.hovered == true){
        change_scene(create_setting_scene());
        al_rest(0.2);
    }
    if (mouseState.buttons && leaderboard.hovered == true) {
        change_scene(create_leaderboard_scene());
        al_rest(0.2);
    }
}

static void draw(void) {
    // Title Text
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        225,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        220,
        ALLEGRO_ALIGN_CENTER,
        "NTHU-RPG ADVENTURE"
    );

    // button
    draw_button(settingButton);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        540 + 28 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTINGS"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        540 + 31 + settingButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "SETTINGS"
    );
    // button
    draw_button(start);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        435 + 28 + start.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "START"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        435 + 31 + start.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "START"
    );
    // button
    draw_button(leaderboard);
    // button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        645 + 28 + leaderboard.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        645 + 31 + leaderboard.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "LEADERBOARD"
    );
}

static void destroy(void) {
    destroy_button(&settingButton);
}


Scene create_menu_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "menu";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
