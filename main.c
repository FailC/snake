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


    Vector2 offsets[4] = {
        { -SCREEN_WIDTH, 0 },
        { SCREEN_WIDTH, 0 },
        { 0, -SCREEN_HEIGHT },
        { 0, SCREEN_HEIGHT }
    };


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

        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (!game_is_over) {
                DrawRectangleRec(player.rect, BLACK);
            } else {
                DrawRectangleRec(player.rect, RED);
            }

            for (int i = 0; i < 4; ++i) {
                Vector2 offset = offsets[i];
                Rectangle ghost = player.rect;
                ghost.x += offset.x;
                ghost.y += offset.y;

                // Only draw if player is near the screen edge in this direction
                if (ghost.x + rectSize > 0 && ghost.x < SCREEN_WIDTH &&
                    ghost.y + rectSize > 0 && ghost.y < SCREEN_HEIGHT) {
                    DrawRectangleRec(ghost, BLACK);

                    if (CheckCollisionRecs(ghost, eating_rect)) {
                        spawn = true;
                        score = true;
                    }
                }
            }

            for (int i = 0; i < player.score; ++i) {
                int delay = follower_delay[i];
                if (delay < SIZE) {
                    follower_pos[i] = get_history_pos(delay);

                    Rectangle temp = {
                        follower_pos[i].x,
                        follower_pos[i].y,
                        GRIDSIZE,
                        GRIDSIZE
                    };
                    Color color = game_is_over ? RED : DARKGRAY;

                    DrawRectangleRec(temp, color);

                    Vector2 wrapOffsets[4] = {
                        { -SCREEN_WIDTH, 0 },
                        { SCREEN_WIDTH, 0 },
                        { 0, -SCREEN_HEIGHT },
                        { 0, SCREEN_HEIGHT }
                    };

                    for (int j = 0; j < 4; j++) {
                        Vector2 offset = wrapOffsets[j];
                        Rectangle ghost = {
                            temp.x + offset.x,
                            temp.y + offset.y,
                                GRIDSIZE,
                            GRIDSIZE
                        };
                        if (ghost.x + GRIDSIZE > 0 && ghost.x < SCREEN_WIDTH &&
                            ghost.y + GRIDSIZE > 0 && ghost.y < SCREEN_HEIGHT) {
                            DrawRectangleRec(ghost, color);
                        }
                    }
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
