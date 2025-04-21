#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500;

const int GRIDSIZE = 50;

typedef struct {
    Rectangle rect;
    int score;
    const float PLAYER_SPEED;
} Player;


int move_key = KEY_DOWN;

void move_player(Player *player) {
    static int old_key;
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

int GetRandomDivisible(int divisor, int min, int max) {
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
    Rectangle rect = { rand_value_x, rand_value_y, GRIDSIZE, GRIDSIZE };
    return rect;
}


#define SIZE 500
Vector2 pos_history[SIZE];
int index_g = 0;
bool history_full = false;

void save_pos(Vector2 pos) {
    pos_history[index_g] = pos;
    index_g = (index_g + 1) % SIZE;
    // if (index_g == 0) history_full = true;
}

Vector2 get_history_pos(int steps_back) {
    int i = (index_g - steps_back + SIZE) % SIZE;
    return pos_history[i];
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "solid snake!!");
    SetTargetFPS(60);
    // SetTargetFPS(10);
                      // pos,   width
    Rectangle p_rect = { 50, 50, GRIDSIZE, GRIDSIZE };
    const float SPEED = 5.0f;
    Player player = {p_rect, .score = 0, SPEED};

    Rectangle eating_rect = spawn_block();
    Rectangle ghost_rect = { 0, 0, GRIDSIZE, GRIDSIZE };

    // int steps_behind = GRIDSIZE / SPEED;
    int size = 30;
    Vector2 pos[size];

    Vector2 follower_pos[player.score];
    int follower_delay[player.score];
    for (int i = 1; i <= player.score; ++i) {
        follower_delay[i] = 10 * i;
        printf("delay: %d\n", follower_delay[i]);
    }

    bool spawn = false;
    int rectSize = player.rect.height;

    while (!WindowShouldClose()) {

        spawn = false;
        // changes rect.x, rect.y
        move_player(&player);
        wrap_player(&player);
        save_pos((Vector2){ player.rect.x, player.rect.y });

        // printf("%f : %f\n", player.rect.x, player.rect.y);

        if (CheckCollisionRecs(player.rect, eating_rect)) {
            eating_rect = spawn_block();
            player.score++;
            spawn = true;
        }
        // create ghost rect for drawing
        if (player.rect.x + rectSize > SCREEN_WIDTH) {
            ghost_rect.x = player.rect.x - SCREEN_WIDTH;
            ghost_rect.y = player.rect.y;
        }
        if (player.rect.x < rectSize) {
            ghost_rect.x = player.rect.x + SCREEN_WIDTH;
            ghost_rect.y = player.rect.y;
        }
        if (player.rect.y + rectSize > SCREEN_HEIGHT) {
            ghost_rect.x = player.rect.x;
            ghost_rect.y = player.rect.y - SCREEN_HEIGHT;
        }
        if (player.rect.y < rectSize) {
            ghost_rect.x = player.rect.x;
            ghost_rect.y = player.rect.y + SCREEN_HEIGHT;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player.rect, BLACK);
            // DrawRectangleRec(second_rect, RED);
            // DrawRectangleRec(rect_array[0], BLACK);

            // :thinking:
            if (player.rect.x + rectSize > SCREEN_WIDTH) {
                DrawRectangleRec(ghost_rect, BLACK);
                if (CheckCollisionRecs(ghost_rect, eating_rect)) {
                    eating_rect= spawn_block();
                    spawn = true;
                }
            }
            if (player.rect.x < rectSize) {
                DrawRectangleRec(ghost_rect, BLACK);
                if (CheckCollisionRecs(ghost_rect, eating_rect)) {
                    eating_rect = spawn_block();
                    spawn = true;
                }
            }
            if (player.rect.y + rectSize > SCREEN_HEIGHT) {
                DrawRectangleRec(ghost_rect, BLACK);
                if (CheckCollisionRecs(ghost_rect, eating_rect)) {
                    eating_rect = spawn_block();
                    spawn = true;
                }
            }
            if (player.rect.y < rectSize) {
                DrawRectangleRec(ghost_rect, BLACK);
                if (CheckCollisionRecs(ghost_rect, eating_rect)) {
                    eating_rect = spawn_block();
                    spawn = true;
                }
            }

            for (int i = 0; i < player.score; ++i) {
                int delay = follower_delay[i];
                if (delay < SIZE) {
                    follower_pos[i] = get_history_pos(delay);
                    DrawRectangleV(follower_pos[i], (Vector2){ GRIDSIZE, GRIDSIZE}, DARKGRAY);
                }
            }

            if (spawn) {
            // spawn more blocks
            }

            DrawRectangleRec(eating_rect, RED);
            DrawFPS(10, 10);

            // char buffer[50];
            // snprintf(buffer, sizeof(buffer), "%d" , player.score);
            // DrawText(buffer,(float)SCREEN_WIDTH/2 - 50, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
