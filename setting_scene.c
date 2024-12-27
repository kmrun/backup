#include <allegro5/allegro.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static ALLEGRO_BITMAP* SLIDER_bitmap;
static ALLEGRO_BITMAP* OTHERSLIDER_bitmap;
static ALLEGRO_BITMAP* AUDIO_bitmap;
static ALLEGRO_BITMAP* AUDIOoff_bitmap;
float pos1 = 400;
float pos2 = 400;


static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    SLIDER_bitmap = al_load_bitmap("Assets/slider.png");
    OTHERSLIDER_bitmap = al_load_bitmap("Assets/slider_hovered.png");
    AUDIO_bitmap = al_load_bitmap("Assets/AUDIO_ON.png");
    AUDIOoff_bitmap = al_load_bitmap("Assets/AUDIO_OFF.png");
}

static void update(void) {

    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
        al_rest(0.2);
    }
    //mutebutton bgm
    if (mouseState.buttons &&
        mouseState.x >= 350 &&
        mouseState.x <= 398 &&
        mouseState.y >= 80 &&
        mouseState.y <= 128) {
        if (pos1 > 100) {
            pos1 = 100;
            BGM_VOLUME = ((float)(pos1 - 100) / 600.0f) * 1.5f;
        }
        else {
            pos1 = 400;
            BGM_VOLUME = ((float)(pos1 - 100) / 600.0f) * 1.5f;
        }
        al_rest(0.3);
    }

    //mutebutton sfx
    if (mouseState.buttons &&
        mouseState.x >= 270 &&
        mouseState.x <= 318&&
        mouseState.y >= 220 &&
        mouseState.y <= 268) {
        if (pos2 > 100) {
            pos2 = 100;
            BGM_VOLUME = ((float)(pos2 - 100) / 600.0f) * 1.5f;
        }
        else {
            pos2 = 400;
            BGM_VOLUME = ((float)(pos2 - 100) / 600.0f) * 1.5f;
        }
        al_rest(0.3);
    }

    if (mouseState.buttons &&
        mouseState.x >= 70 &&
        mouseState.y >= 120 &&
        mouseState.x <= 730 &&
        mouseState.y <= 210) {
        pos1 = mouseState.x;
        if (pos1 < 100) {
            pos1 = 100;
        }
        if (pos1 > 700) {
            pos1 = 700;
        }
        BGM_VOLUME = ((float)(pos1 - 100) / 600.0f)*1.5f; //ratio
    }
    if (mouseState.buttons &&
        mouseState.x >= 70 &&
        mouseState.y >= 265 &&
        mouseState.x <= 730 &&
        mouseState.y <= 355) {
        pos2 = mouseState.x;
        if (pos2 < 100) {
            pos2 = 100;
        }
        if (pos2 > 700) {
            pos2 = 700;
        }
        SFX_VOLUME = ((float)((pos2 - 100) / 600.0f))*1.5f; //ratio
    }
}

static void draw(void) {
    // button
    draw_button(backButton);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        650 + 28 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "APPLY & BACK"
    );
    //audio button
    ALLEGRO_BITMAP* audioimage = (pos1 == 100) ? AUDIOoff_bitmap: AUDIO_bitmap;
    al_draw_scaled_bitmap(audioimage,
        0, 0, 16, 16,
        350, 80, 16*3, 16*3, 0
    );
    ALLEGRO_BITMAP* audioimage2 = (pos2 == 100) ? AUDIOoff_bitmap : AUDIO_bitmap;
    al_draw_scaled_bitmap(audioimage2,
        0, 0, 16, 16,
        270, 220, 16 * 3, 16 * 3, 0
    );

    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        650 + 31 + backButton.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "APPLY & BACK"
    );

    //volume sliders BGM
    al_draw_text(
        P2_FONT,
        al_map_rgb(146, 161, 185),
        100,
        93,
        ALLEGRO_ALIGN_LEFT,
        "MUSIKAaaaa"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        100,
        90,
        ALLEGRO_ALIGN_LEFT,
        "MUSIKAaaaa"
    );

    al_draw_scaled_bitmap(SLIDER_bitmap,
        0, 0, 600, 20,
        100, 150, 600, 30, 0
    );
    al_draw_scaled_bitmap(OTHERSLIDER_bitmap,
        0, 0, 600, 20,
        100, 150, pos1-100, 30, 0
    );

    //sliders SFX
    al_draw_text(
        P2_FONT,
        al_map_rgb(146, 161, 185),
        100,
        238,
        ALLEGRO_ALIGN_LEFT,
        "SFSXFS"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        100,
        235,
        ALLEGRO_ALIGN_LEFT,
        "SFSXFS"
    );
    al_draw_scaled_bitmap(SLIDER_bitmap,
        0, 0, 600, 20,
        100, 295, 600, 30, 0
    );
    al_draw_scaled_bitmap(OTHERSLIDER_bitmap,
        0, 0, 600, 20,
        100, 295, pos2-100, 30, 0
    );
}

static void destroy(void) {
    destroy_button(&backButton);
}

Scene create_setting_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "setting";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
