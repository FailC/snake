#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGT = 500;

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

    bool move_left_right = (int)player->rect.y % 10 == 0;
    bool move_up_down = (int)player->rect.x % 10 == 0;

    printf("current: %d pressed: %d\n", move_key, old_key);

    switch (new_key) {
        case KEY_UP:
            if (move_key != KEY_DOWN && move_up_down)  {
                // switch to move up
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
    }

}


void _move_player(Player *player) {
    static int old_key;

    if (!(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) && ((IsKeyDown(KEY_UP) || old_key == KEY_UP) && !(IsKeyDown(KEY_DOWN)))) {
        if ((player->rect.y != 0) && (int)player->rect.x % 10 == 0) {
            player->rect.y -= player->PLAYER_SPEED;
            old_key = KEY_UP;
        }
    }

    if (!(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) && ((IsKeyDown(KEY_DOWN) ||  old_key == KEY_DOWN) && !(IsKeyDown(KEY_UP)))) {
        if ((player->rect.x != 0) && (int)player->rect.x % 10 == 0) {
            player->rect.y += player->PLAYER_SPEED;
            old_key = KEY_DOWN;
        }
    }
    if (!(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP)) && (IsKeyDown(KEY_RIGHT) || old_key == KEY_RIGHT)) {
        if ((player->rect.y != 0) && (int)player->rect.y % 10 == 0) {
            player->rect.x += player->PLAYER_SPEED;
            old_key = KEY_RIGHT;
        }
    }
    if (!(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_UP)) && (IsKeyDown(KEY_LEFT)|| old_key == KEY_LEFT)) {
        if ((player->rect.y != 0) && (int)player->rect.y % 10 == 0) {
            player->rect.x -= player->PLAYER_SPEED;
            old_key = KEY_LEFT;
        }
    }
}



Rectangle spawn_block() {
    // spawn random cube
    int rand_value_x = GetRandomValue(10, SCREEN_WIDTH - 10);
    int rand_value_y = GetRandomValue(10, SCREEN_HEIGT - 10);
    Rectangle rect = {rand_value_x, rand_value_y, 20, 20};

    return rect;
}

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGT, "snake!!");
    SetTargetFPS(30);

    Rectangle p_rect = {50,50,20.0f,20.0f};
    Player player = {p_rect, 0, 5.0f};

    Rectangle rect = spawn_block();
    while (!WindowShouldClose()) {

        move_player(&player);

        // printf("pos: %f : %f\n", player.rect.x, player.rect.y);

        // switch sides if out of bounds

        //printf("%d : %d\n", player.score, player_2.score);
        // if (CheckCollisionRecs(player.rect, ball_rect)) {
        //     collision = true;
        //     printf("COLLISION!!\n");


        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawRectangleRec(player.rect, BLACK);

            DrawRectangleRec(rect, RED);

            // char buffer[50];
            // snprintf(buffer, sizeof(buffer), "%d" , player.score);
            // DrawText(buffer,(float)SCREEN_WIDTH/2 - 50, 10, 20, BLACK);
            // DrawText(buffer_2, (float)SCREEN_WIDTH/2 + 50, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
