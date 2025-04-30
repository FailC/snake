// #include <stdio.h>
#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include "lib.c"

#define FOLLOWER 250


static int head = 0;
static int tail = 0;

void check_direction_change(const Player *player, const posHistory *history, Rectangle fill_blocks[]) {

    static bool direction_changed = false;
    static bool changed_x = false;
    static bool changed_y = true;

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

    if (direction_changed) {
        direction_changed = false;

        // Calculate the ghost position based on the current direction
        Vector2 offset = {0, 0};

        // Adjust the ghost position when the player leaves the right side of the screen
        if (player->rect.x >= SCREEN_WIDTH) {
            offset.x = -SCREEN_WIDTH;
        }
        // Adjust the ghost position when the player leaves the left side of the screen
        else if (player->rect.x < 0) {
            offset.x = SCREEN_WIDTH;
        }

        // Adjust the ghost position when the player leaves the bottom side of the screen
        if (player->rect.y >= SCREEN_HEIGHT) {
            offset.y = -SCREEN_HEIGHT;
        }
        // Adjust the ghost position when the player leaves the top side of the screen
        else if (player->rect.y < 0) {
            offset.y = SCREEN_HEIGHT;
        }

        // Calculate the ghost block's position based on the previous position + the offset
        Rectangle ghost_filler_rec = {
            prev.x + offset.x,
            prev.y + offset.y,
            GRIDSIZE,
            GRIDSIZE
        };

        // Store the ghost filler rectangle at the calculated position
        fill_blocks[head++] = ghost_filler_rec;
        if (head >= SIZE_FILL_BLOCK) {
            head = 0;
        }
    }
}


void draw_filler(Rectangle fill_blocks[]) {
    int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);
    for (int n = 0; n < buffer_active_count; ++n) {
        int index = (tail + n) % SIZE_FILL_BLOCK;
        DrawRectangleRec(fill_blocks[index], DARKGRAY);
    }
}


int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "solid snake!!");
    SetTargetFPS(60);
    // SetTargetFPS(10);

    // player
    Rectangle p_rect = { 50, 50, GRIDSIZE, GRIDSIZE };
    Player player = {p_rect, .score = 2, SPEED};
    int highscore = 0;

    Rectangle eating_rect;

    int follower_count = FOLLOWER;
    Vector2 follower_pos[follower_count];
    int follower_delay[follower_count];

    for (int i = 0; i < follower_count; ++i) {
        follower_delay[i] = 10 * (i + 1);
    }

    int rectSize = player.rect.height;
    bool score = false;
    bool spawn_eating_rect = true;
    bool game_is_over = false;

    Rectangle fill_blocks[SIZE_FILL_BLOCK];

    Vector2 offsets[4] = {
        { -SCREEN_WIDTH, 0 },
        { SCREEN_WIDTH, 0 },
        { 0, -SCREEN_HEIGHT },
        { 0, SCREEN_HEIGHT }
    };

    posHistory history = {.index = 0};

    while (!WindowShouldClose()) {

        // changes rect.x, rect.y
        move_player(&player);
        wrap_player(&player);
        save_pos(&history, (Vector2){ player.rect.x, player.rect.y });

        if (game_is_over && IsKeyPressed(KEY_SPACE)) {
            game_restart(&player, &history);
            score = false;
            spawn_eating_rect = true;
            game_is_over = false;
        }

        // printf("%f : %f\n", player.rect.x, player.rect.y);

        if (CheckCollisionRecs(player.rect, eating_rect)) {
            spawn_eating_rect = true;
            score = true;
        }

        // important function, stores the fill blocks
        if (player.score >= 1) {
            check_direction_change(&player, &history, fill_blocks);
            // printf("head: %d\n", head);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!game_is_over) {
            DrawRectangleRec(player.rect, BLACK);
        } else {
            DrawRectangleRec(player.rect, RED);
        }

        // draw ghost blocks out of bounds
        for (int i = 0; i < 4; ++i) {
            Vector2 offset = offsets[i];
            Rectangle ghost = player.rect;
            ghost.x += offset.x;
            ghost.y += offset.y;

            // Only draw if player is near the screen edge in this direction
            if (ghost.x + rectSize > 0 && ghost.x < SCREEN_WIDTH &&
                ghost.y + rectSize > 0 && ghost.y < SCREEN_HEIGHT) {
                DrawRectangleRec(ghost, GREEN);

                if (CheckCollisionRecs(ghost, eating_rect)) {
                    spawn_eating_rect = true;
                    score = true;
                }
            }
        }

        // draw follow blocks
        // for the last index, check collision with fill_blocks
        for (int i = 0; i < player.score; ++i) {
            int delay = follower_delay[i];
            if (delay < HISTORY_SIZE) {
                follower_pos[i] = get_history_pos(&history,delay);

                Rectangle temp = {
                    follower_pos[i].x,
                    follower_pos[i].y,
                    GRIDSIZE,
                    GRIDSIZE
                };
                Color color = game_is_over ? RED : DARKGRAY;

                if (i == player.score - 1) {
                    int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);
                    // printf("active: %d\n", buffer_active_count);

                    for (int n = 0; n < buffer_active_count; ++n) {
                        int index = (tail + n) % SIZE_FILL_BLOCK;
                        if (CheckCollisionRecs(temp, fill_blocks[index])) {
                            tail = (index + 1) % SIZE_FILL_BLOCK;
                            break;
                        }
                    }
                }

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
                    spawn_eating_rect = true;
                }
                if (i != 0 && CheckCollisionRecs(temp, player.rect)) {
                    // debug, turn off collision
                    // game_over(&player, highscore);
                    // game_is_over = true;
                }
            }
        }

        // draw fill blocks
        draw_filler(fill_blocks);

        if (score) player.score++;
        if (spawn_eating_rect) eating_rect = spawn_block();
        DrawRectangleRec(eating_rect, RED);
        DrawFPS(10, 10);
        spawn_eating_rect = false;
        score = false;

        if (game_is_over) {
            DrawText("GAME OVER!", 100, 100, 20, BLACK);
        }

        draw_int_to_text(player.score * 100, 150, 10);
        draw_int_to_text(highscore * 100, 500, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

