#include <stdio.h>
#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include "lib.c"

#define FOLLOWER 250
#define SIZE 2500
Vector2 pos_history[SIZE];
int index_g = 0;

void save_pos(Vector2 pos) {
    pos_history[index_g] = pos;
    index_g = (index_g + 1) % SIZE;
}

Vector2 get_history_pos(int steps_back) {
    int i = (index_g - steps_back + SIZE) % SIZE;
    return pos_history[i];
}

void game_over(Player *player) {
    player->PLAYER_SPEED = 0;
}

float SPEED = 5.0f;

void game_restart(Player *player) {
    // pos_history
    // bool
    // index_g
    // player, position, speed

    index_g = 0;
    for (int i = 0; i < SIZE; ++i) {
        pos_history[i] = (Vector2) {0.0f, 0.0f };
    }
    player->score = 0;
    player->PLAYER_SPEED = SPEED;

}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "solid snake!!");
    SetTargetFPS(60);
    // SetTargetFPS(10);
    printf("Hello solid snake!\n");

    Rectangle p_rect = { 50, 50, GRIDSIZE, GRIDSIZE };
    Player player = {p_rect, .score = 0, SPEED};

    Rectangle eating_rect;
    Rectangle ghost_rect = { 0, 0, GRIDSIZE, GRIDSIZE };

    int follower_count = FOLLOWER;
    Vector2 follower_pos[follower_count];
    int follower_delay[follower_count];

    for (int i = 0; i < follower_count; ++i) {
        follower_delay[i] = 10 * (i + 1);
    }

    int rectSize = player.rect.height;
    bool score = false;
    bool spawn = true;
    bool game_is_over = false;

    while (!WindowShouldClose()) {

        // changes rect.x, rect.y
        move_player(&player);
        wrap_player(&player);
        save_pos((Vector2){ player.rect.x, player.rect.y });

        if (game_is_over && IsKeyPressed(KEY_SPACE)) {
            game_restart(&player);
            

            score = false;
            spawn = true;
            game_is_over = false;
        }

        // printf("%f : %f\n", player.rect.x, player.rect.y);

        if (CheckCollisionRecs(player.rect, eating_rect)) {
            spawn = true;
            score = true;
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

            if (!game_is_over) {
                DrawRectangleRec(player.rect, BLACK);
            } else {
                DrawRectangleRec(player.rect, RED);
            }

            if (player.rect.x + rectSize > SCREEN_WIDTH ||
                player.rect.x < rectSize ||
                player.rect.y + rectSize > SCREEN_HEIGHT ||
                player.rect.y < rectSize) {
                DrawRectangleRec(ghost_rect, BLACK);
                if (CheckCollisionRecs(ghost_rect, eating_rect)) {
                    spawn = true;
                    score = true;
                }
            }

            for (int i = 0; i < player.score; ++i) {
                int delay = follower_delay[i];
                if (delay < SIZE) {
                    follower_pos[i] = get_history_pos(delay);
                    if (!game_is_over) {
                        DrawRectangleV(follower_pos[i], (Vector2){ GRIDSIZE, GRIDSIZE}, DARKGRAY);
                    } else {
                        DrawRectangleV(follower_pos[i], (Vector2){ GRIDSIZE, GRIDSIZE}, RED);
                    }
                    Rectangle temp = {follower_pos[i].x, follower_pos[i].y, GRIDSIZE, GRIDSIZE};
                    if (CheckCollisionRecs(temp, eating_rect)) {
                        score = true;
                        spawn = true;
                    }
                    if (i != 0 && CheckCollisionRecs(temp, player.rect)) {
                        game_over(&player);
                        game_is_over = true;
                    }
                }
            }
            if (score) player.score++;
            if (spawn) eating_rect = spawn_block();
            DrawRectangleRec(eating_rect, RED);
            DrawFPS(10, 10);
            spawn = false;
            score = false;

            if (game_is_over) {
                DrawText("GAME OVER!",100,100, 20, BLACK);
            }

            char buffer[50];
            snprintf(buffer, sizeof(buffer), "%d" , player.score * 100);
            DrawText(buffer, 150, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
