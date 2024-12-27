#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "setting_scene.h"
#include "menu_scene.h"
#include "utility.h"
#include "UI.h"
#include "game.h"

static Button backButton;
static ALLEGRO_BITMAP* canvas;

#define MAX 10

typedef struct {
    char string[10];
    int thescore;
} leaderboardEntries;

static leaderboardEntries leaderboard[MAX];
static int leaderboardSize = 0; // Number of entries loaded

static void init(void) {
    backButton = button_create(SCREEN_W / 2 - 200, 675, 400, 100, "Assets/UI_Button.png", "Assets/UI_Button_hovered2.png");
    canvas = al_load_bitmap("Assets/Carved_9Slides.png");

    FILE* f = NULL;
    fopen_s(&f, "Assets/leaderboard.txt", "r");
    if (f) {
        leaderboardSize = 0;
        while (leaderboardSize < MAX && fscanf_s(f, "%9s %d", leaderboard[leaderboardSize].string, (unsigned)_countof(leaderboard[leaderboardSize].string), &leaderboard[leaderboardSize].thescore) == 2) {
            leaderboardSize++;
        }
        fclose(f);
    }
    for (int i = 0; i < leaderboardSize - 1; i++) {
        for (int j = 0; j < leaderboardSize - i - 1; j++) {
            if (leaderboard[j].thescore < leaderboard[j + 1].thescore) {
                // Swap entries
                leaderboardEntries temp = leaderboard[j];
                leaderboard[j] = leaderboard[j + 1];
                leaderboard[j + 1] = temp;
            }
        }
    }
}

static void update(void) {
    update_button(&backButton);

    if (mouseState.buttons && backButton.hovered == true) {
        change_scene(create_menu_scene());
        al_rest(0.2);
        return;
    }
}

static void draw(void) {
    al_draw_scaled_bitmap(canvas, 0, 0, 192, 192, 10, 125, 780, 530, 0);

    // Draw button
    draw_button(backButton);

    // Button text
    al_draw_text(P2_FONT, al_map_rgb(66, 76, 110), SCREEN_W / 2, 675 + 28 + backButton.hovered * 15, ALLEGRO_ALIGN_CENTER, "BACK");
    al_draw_text(P2_FONT, al_map_rgb(225, 225, 225), SCREEN_W / 2, 675 + 31 + backButton.hovered * 15, ALLEGRO_ALIGN_CENTER, "BACK");

    // Title
    al_draw_text(TITLE_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 16, ALLEGRO_ALIGN_CENTER, "LEADERBOARD");

    // Draw leaderboard entries
    for (int i = 0; i < leaderboardSize; i++) {
        char entry[50];
        snprintf(entry, sizeof(entry), "%2d. %-10s %5d", i + 1, leaderboard[i].string, leaderboard[i].thescore);
        al_draw_text(P2_FONT, al_map_rgb(255, 255, 255), SCREEN_W / 2, 200 + i * 40, ALLEGRO_ALIGN_CENTER, entry);
    }
}

static void destroy(void) {
    destroy_button(&backButton);
    al_destroy_bitmap(canvas);
}

Scene create_leaderboard_scene(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));

    scene.name = "leaderboard";
    scene.init = &init;
    scene.draw = &draw;
    scene.update = &update;
    scene.destroy = &destroy;

    return scene;
}
