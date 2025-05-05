#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 500
#define GRIDSIZE 50
#define HISTORY_SIZE 2500
#define SIZE_FILL_BLOCK 50
#define SPEED 5.0f

int move_key = KEY_DOWN;


int load_file() {
    FILE *file;
    char filename[] = "highscore.txt";
    char buffer[12];

    file = fopen(filename, "r");  // Open the file in read mode

    if (file == NULL) {
        printf("cant open file");
        return 0;
    }
    fgets(buffer, sizeof(buffer), file);

    fclose(file);
    int highscore = atoi(buffer);
    return highscore;
}

void game_restart(Player *player, posHistory *history) {
    history->index = 0;
    for (int i = 0; i < HISTORY_SIZE; ++i) {
        history->positions[i]= (Vector2) {0.0f, 0.0f };
    }
    player->score = 2;
    player->PLAYER_SPEED = SPEED;
}

void game_over(Player *player, int *highscore) {
    player->PLAYER_SPEED = 0;
    if (player->score > *highscore) {
        *highscore = player->score;
    }
    // write to file
    FILE *file;
    char *filename = "highscore.txt";
    file = fopen(filename, "w");
    if (!file) {
        return;
    }
    fprintf(file, "%d", *highscore);
}

void save_pos(posHistory *history,Vector2 const pos) {
    history->positions[history->index] = pos;
    history->index = (history->index + 1) % HISTORY_SIZE;
}

Vector2 get_prev_pos(const posHistory *history, const int steps_back) {
    int i = (history->index - steps_back + HISTORY_SIZE) % HISTORY_SIZE;
    return history->positions[i];
}

void draw_int_to_text(const int element, const int posX, const int posY) {
    int fontsize = 20;
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%d", element);
    DrawText(buffer,posX, posY, fontsize, BLACK);
}

void insert_fill_block(Player *player, const posHistory *history, Rectangle fill_blocks[], int *head) {
    Vector2 offset = {0, 0};
    Vector2 prev = get_prev_pos(history, 2);
    Rectangle ghost_filler_rec = {
        prev.x,
        prev.y,
        GRIDSIZE,
        GRIDSIZE
    };
    // printf debug
    // printf("prev position-> %f : %f\n", prev.x, prev.y);
    // printf("ghost filler-> %f : %f\n", ghost_filler_rec.x, ghost_filler_rec.y);

    fill_blocks[(*head)++] = ghost_filler_rec;
    if (*head >= SIZE_FILL_BLOCK) {
        *head = 0;
    }
}


void draw_filler(Rectangle fill_blocks[], int *tail_p, int *head_p) {
    // meh
    int tail = *tail_p;
    int head = *head_p;
    int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);
    for (int n = 0; n < buffer_active_count; ++n) {
        int index = (tail + n) % SIZE_FILL_BLOCK;
        DrawRectangleRec(fill_blocks[index], DARKGRAY);
        // DrawRectangleRec(fill_blocks[index], GREEN);
    }
}

bool direction_change(const posHistory *history) {
    static bool changed_x = false;
    static bool changed_y = true;

    bool direction_changed = false;

    Vector2 curr = get_prev_pos(history, 1);
    Vector2 prev = get_prev_pos(history, 2);

    // Handle direction change for moving in the Y-axis
    if ((curr.x == prev.x) && (curr.y != prev.y) && changed_y) {
        changed_x = true;
        changed_y = false;
        direction_changed = true;
    }
    // Handle direction change for moving in the X-axis
    if ((curr.x != prev.x) && (curr.y == prev.y) && changed_x) {
        changed_y = true;
        changed_x = false;
        direction_changed = true;
    }

    return direction_changed;
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
            if (move_key != KEY_DOWN && move_up_down) move_key = KEY_UP;
            break;
        case KEY_DOWN:
            if (move_key != KEY_UP && move_up_down) move_key = KEY_DOWN;
            break;
        case KEY_LEFT:
            if (move_key != KEY_RIGHT && move_left_right) move_key = KEY_LEFT;
            break;
        case KEY_RIGHT:
            if (move_key != KEY_LEFT && move_left_right) move_key = KEY_RIGHT;
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
    if (player->rect.x >= SCREEN_WIDTH) player->rect.x = 0;
    if (player->rect.x <= -player->rect.width) player->rect.x = SCREEN_WIDTH - rectSize;
    if (player->rect.y >= SCREEN_HEIGHT) player->rect.y = 0;
    if (player->rect.y <= -player->rect.height) player->rect.y = SCREEN_HEIGHT - rectSize;
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

