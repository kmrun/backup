#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <string.h>
#include "win_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"
#include "game_scene.h"
#include "player.h"

static ALLEGRO_EVENT_QUEUE* event_queue; // Declare the event queue
static Button upload;
static Button backButton;
static ALLEGRO_BITMAP* win_bitmap;
static char input[11] = ""; // Stores the player's name (max 10 characters + \0)
static int chars = 0;        // Tracks the current index for input

extern int remaininghp;
extern int coins_obtained;
int score;

static void init(void) {
    al_rest(0.2);
    score = remaininghp * 10 + coins_obtained * 10;
    backButton = button_create(SCREEN_W / 2 - 200, 650, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    upload = button_create(SCREEN_W / 2 - 200, 540, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered.png");
    win_bitmap = al_load_bitmap("Assets/panda_win.png");

    // Initialize the event queue
    event_queue = al_create_event_queue();

    // Register event sources
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
}

static void update(void) {
    update_button(&backButton);
    update_button(&upload);

    ALLEGRO_EVENT ev;
    while (al_get_next_event(event_queue, &ev)) {
        if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && chars > 0) {
                input[--chars] = '\0';
            }
            else if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar <= 126 && chars < 10) {
                input[chars++] = (char)ev.keyboard.unichar;
                input[chars] = '\0';
            }
        }
    }

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
        coins_obtained = 0;
    }

    if (mouseState.buttons && upload.hovered == true) {
        FILE* file = fopen("Assets/leaderboard.txt", "a");
        if (file) {
            fprintf(file, "%s %d\n", input, score);
            fclose(file);
        }
        al_rest(0.2);
    }
}

static void draw(void) {
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        SCREEN_H / 12,
        ALLEGRO_ALIGN_CENTER,
        "YOU WIN"
    );
    al_draw_text(
        TITLE_FONT,
        al_map_rgb(199, 207, 221),
        SCREEN_W / 2,
        SCREEN_H / 12 - 5,
        ALLEGRO_ALIGN_CENTER,
        "YOU WIN"
    );

    al_draw_textf(
        P2_FONT,
        al_map_rgb(199, 207, 221),
        10,
        10,
        ALLEGRO_ALIGN_LEFT,
        "YOUR SCORE: %d", score
    );

    al_draw_scaled_bitmap(win_bitmap,
        0, 0, 64, 64,
        SCREEN_W / 4, SCREEN_H / 12, 64 * 6, 64 * 6, 0);

    // Draw input prompt
    al_draw_text(
        P2_FONT,
        al_map_rgb(146, 161, 185),
        SCREEN_W / 2,
        400,
        ALLEGRO_ALIGN_CENTER,
        "Enter Your Name:"
    );
    al_draw_textf(
        P2_FONT,
        al_map_rgb(255, 255, 255),
        SCREEN_W / 2,
        450,
        ALLEGRO_ALIGN_CENTER,
        "%s", input
    );

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

    draw_button(upload);
    //button text
    al_draw_text(
        P2_FONT,
        al_map_rgb(66, 76, 110),
        SCREEN_W / 2,
        540 + 28 + upload.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "UPLOAD"
    );
    al_draw_text(
        P2_FONT,
        al_map_rgb(225, 225, 225),
        SCREEN_W / 2,
        540 + 31 + upload.hovered * 11,
        ALLEGRO_ALIGN_CENTER,
        "UPLOAD"
    );
}

static void destroy(void) {
    destroy_button(&backButton);
    destroy_button(&upload);
    if (win_bitmap) {
        al_destroy_bitmap(win_bitmap);
    }

    // Destroy the event queue
    if (event_queue) {
        al_destroy_event_queue(event_queue);
    }
}


Scene create_win_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "win";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
