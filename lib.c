#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 500
#define GRIDSIZE 50

int move_key = KEY_DOWN;

typedef struct {
    Rectangle rect;
    int score;
    float PLAYER_SPEED;
} Player;

void draw_int_to_text(const int element, const int posX, const int posY) {
    int fontsize = 20;
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%d", element);
    DrawText(buffer,posX, posY, fontsize, BLACK);
}

void move_player(Player *player) {
    static int new_key;
    static int key;

    key = GetKeyPressed();
    if (key != 0) {
        new_key = key;
    }
    bool move_left_right = (int)player->rect.y % GRIDSIZE == 0;
    bool move_up_down = (int)player->rect.x % GRIDSIZE == 0;
    switch (new_key) {
        case KEY_UP:
            if (move_key != KEY_DOWN && move_up_down)  {
                move_key = KEY_UP;
            }
            break;
        case KEY_DOWN:
            if (move_key != KEY_UP && move_up_down) {
                move_key = KEY_DOWN;
            }
            break;
        case KEY_LEFT:
            if (move_key != KEY_RIGHT && move_left_right) {
                move_key = KEY_LEFT;
            }
            break;
        case KEY_RIGHT:
            if (move_key != KEY_LEFT && move_left_right) {
                move_key = KEY_RIGHT;
            }
            break;
        default:
            break;
    }
    switch (move_key) {
        case KEY_UP:
                player->rect.y -= player->PLAYER_SPEED;
            break;
        case KEY_DOWN:
                player->rect.y += player->PLAYER_SPEED;
            break;
        case KEY_LEFT:
                player->rect.x -= player->PLAYER_SPEED;
            break;
        case KEY_RIGHT:
                player->rect.x += player->PLAYER_SPEED;
            break;
        default:
            break;
    }
}

void wrap_player(Player *player) {
    int rectSize = player->rect.height;
    if (player->rect.x > SCREEN_WIDTH) player->rect.x = 0;
    if (player->rect.x < -player->rect.width) player->rect.x = SCREEN_WIDTH - rectSize;
    if (player->rect.y > SCREEN_HEIGHT) player->rect.y = 0;
    if (player->rect.y < -player->rect.height) player->rect.y = SCREEN_HEIGHT - rectSize;
}

int GetRandomDivisible(const int divisor, const int min, const int max) {
    int start = (min + divisor - 1) / divisor; // ceil(min / divisor)
    int end = max / divisor;                  // floor(max / divisor)
    if (start > end) return -1; // No valid number
    int randomMultiple = GetRandomValue(start, end);
    return randomMultiple * divisor;
}

Rectangle spawn_block() {
    // spawn random cube
    int rand_value_x = GetRandomDivisible(GRIDSIZE, 10, SCREEN_WIDTH - 10);
    int rand_value_y = GetRandomDivisible(GRIDSIZE, 10, SCREEN_HEIGHT - 10);
    Rectangle rect = { (float)rand_value_x, (float)rand_value_y, GRIDSIZE, GRIDSIZE };
    return rect;
}

