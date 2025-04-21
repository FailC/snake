#include <stdio.h>
#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include "lib.c"


#define SIZE 500
Vector2 pos_history[SIZE];
int index_g = 0;
bool history_full = false;

void save_pos(Vector2 pos) {
    pos_history[index_g] = pos;
    index_g = (index_g + 1) % SIZE;
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

    // Rectangle eating_rect = spawn_block();
    Rectangle eating_rect;

    Rectangle ghost_rect = { 0, 0, GRIDSIZE, GRIDSIZE };

    int follower_count = 500;
    Vector2 follower_pos[follower_count];
    int follower_delay[follower_count];

    for (int i = 0; i < follower_count; ++i) {
        follower_delay[i] = 10 * (i + 1);
    }

    int rectSize = player.rect.height;
    bool score = false;
    bool spawn = true;

    while (!WindowShouldClose()) {

        // changes rect.x, rect.y
        move_player(&player);
        wrap_player(&player);
        save_pos((Vector2){ player.rect.x, player.rect.y });

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

            DrawRectangleRec(player.rect, BLACK);
            // DrawRectangleRec(second_rect, RED);
            // DrawRectangleRec(rect_array[0], BLACK);

            // :thinking:

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
                    DrawRectangleV(follower_pos[i], (Vector2){ GRIDSIZE, GRIDSIZE}, DARKGRAY);
                    Rectangle temp = {follower_pos[i].x, follower_pos[i].y, GRIDSIZE, GRIDSIZE};
                    if (CheckCollisionRecs(temp, eating_rect)) {
                        //eating_rect = spawn_block();
                        score = true;
                        break;
                    }
                    if (i != 0 && CheckCollisionRecs(temp, player.rect)) {
                        // printf("COLLISION\n");
                    }
                }
            }
            if (score) player.score++;
            if (spawn) {
                eating_rect = spawn_block();
            }
            DrawRectangleRec(eating_rect, RED);
            DrawFPS(10, 10);
            spawn = false;
            score = false;

            // char buffer[50];
            // snprintf(buffer, sizeof(buffer), "%d" , player.score);
            // DrawText(buffer,(float)SCREEN_WIDTH/2 - 50, 10, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
