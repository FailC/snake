#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500;

const int GRIDSIZE= 30;

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


int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "snake!!");
    SetTargetFPS(60);
    // SetTargetFPS(10);
                      // pos,   width
    Rectangle p_rect = { 50, 50, GRIDSIZE, GRIDSIZE };
    const float SPEED = 5.0f;
    Player player = {p_rect, .score = 0, SPEED};
    Rectangle rect = spawn_block();

    int x, y;

    int rectSize = player.rect.height;
    while (!WindowShouldClose()) {

        // changes rect.x, rect.y
        move_player(&player);
        wrap_player(&player);

        // debug
        printf("pos: %f : %f\n", player.rect.x, player.rect.y);

        if (CheckCollisionRecs(player.rect, rect)) {
            rect = spawn_block();
            // score++
            // player gets longer
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player.rect, BLACK);
            if (player.rect.x + rectSize > SCREEN_WIDTH) {
                DrawRectangle((int)(player.rect.x - SCREEN_WIDTH), (int)player.rect.y, rectSize, rectSize, BLACK);
            }
            if (player.rect.x < rectSize) {
                DrawRectangle((int)(player.rect.x + SCREEN_WIDTH), (int)player.rect.y, rectSize, rectSize, BLACK);
            }
            if (player.rect.y + rectSize > SCREEN_HEIGHT) {
                DrawRectangle((int)player.rect.x, (int)(player.rect.y - SCREEN_HEIGHT), rectSize, rectSize, BLACK);
            }
            if (player.rect.y < rectSize) {
                DrawRectangle((int)player.rect.x, (int)(player.rect.y + SCREEN_HEIGHT), rectSize, rectSize, BLACK);
            }
            DrawRectangleRec(rect, RED);
            // DrawGrid(10, 10);
            DrawFPS(10, 10);

            // char buffer[50];
            // snprintf(buffer, sizeof(buffer), "%d" , player.score);
            // DrawText(buffer,(float)SCREEN_WIDTH/2 - 50, 10, 20, BLACK);
            // DrawText(buffer_2, (float)SCREEN_WIDTH/2 + 50, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
