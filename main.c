#include <stdio.h>
#include "./raylib-5.5_linux_amd64/include/raylib.h"
#include "lib.c"

#define FOLLOWER 250
// history size:
#define SIZE 2500
Vector2 pos_history[SIZE];
int index_g = 0;

void draw_int_to_text(int element, int posX, int posY) {
    int fontsize = 20;
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%d", element);
    DrawText(buffer,posX, posY, fontsize, BLACK);
}

void save_pos(Vector2 pos) {
    pos_history[index_g] = pos;
    index_g = (index_g + 1) % SIZE;
}

Vector2 get_history_pos(int steps_back) {
    int i = (index_g - steps_back + SIZE) % SIZE;
    return pos_history[i];
}

Vector2 get_prev_pos(int steps_back) {
    int i = (index_g - steps_back + SIZE) % SIZE;
    return pos_history[i];
}

int filler_count = 0;
#define SIZE_FILL_BLOCK 1000
Rectangle fill_blocks[SIZE_FILL_BLOCK];
bool direction_changed = false;
bool changed_x = false;
bool changed_y = true;
Rectangle filler_rec;

int head = 0;
int tail = 0;

void _check_direction_change() {
        Vector2 curr = get_prev_pos(1);
        Vector2 prev = get_prev_pos(2);

        if ((curr.x == prev.x) && (curr.y != prev.y) && changed_y) {
            changed_x = true;
            changed_y = false;
            direction_changed = true;
        }
        if ((curr.x != prev.x) && (curr.y == prev.y) && changed_x) {
            changed_y = true;
            changed_x = false;
            direction_changed = true;
        }

        if (direction_changed) {
            direction_changed = false;
            // move index or something
            filler_rec = (Rectangle){ prev.x, prev.y, GRIDSIZE, GRIDSIZE };
            fill_blocks[head++] = filler_rec;
        }

}


void __check_direction_change(Player *player) {
    Vector2 curr = get_prev_pos(1);
    Vector2 prev = get_prev_pos(2);

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
        if (player->rect.x > SCREEN_WIDTH) {
            offset.x = SCREEN_WIDTH;
        } else if (player->rect.x < 0) {
            offset.x = SCREEN_WIDTH;
        }
        if (player->rect.y > SCREEN_HEIGHT) {
            offset.y = -SCREEN_HEIGHT;
        } else if (player->rect.y < 0) {
            offset.y = SCREEN_HEIGHT;
        }

        // Set the ghost position where the fill block should be drawn
        Rectangle ghost_filler_rec = {
            prev.x + offset.x,
            prev.y + offset.y,
            GRIDSIZE,
            GRIDSIZE
        };

        // Store the ghost filler rectangle at the calculated position
        fill_blocks[head++] = ghost_filler_rec;
    }
}


void check_direction_change(Player *player) {
    Vector2 curr = get_prev_pos(1);
    Vector2 prev = get_prev_pos(2);

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

    }
}


int highscore = 0;
void game_over(Player *player) {
    player->PLAYER_SPEED = 0;
    if (player->score > highscore) {
        highscore = player->score;
    }
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

void draw_filler() {
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
    // printf("Hello solid snake!\n");

    Rectangle p_rect = { 50, 50, GRIDSIZE, GRIDSIZE };
    Player player = {p_rect, .score = 2, SPEED};

    Rectangle eating_rect;

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
    bool pos_changed = true;

    Vector2 last_pos;
    Vector2 current_pos;
    Vector2 last_dir = {1, 0};

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

        Vector2 curr = get_prev_pos(1);
        Vector2 prev = get_prev_pos(2);

        // this is somehow very important..
        // puts the fill blocks in the array
        if (player.score >= 1) {
            check_direction_change(&player);
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
                    spawn = true;
                    score = true;
                }
            }
        }

        // draw follow blocks
        // for the last index, check collision with fill_blocks
        // for (int i = 0; i < player.score; ++i) {
        //     int delay = follower_delay[i];
        //     if (delay < SIZE) {
        //         follower_pos[i] = get_history_pos(delay);
        //
        //         Rectangle temp = {
        //             follower_pos[i].x,
        //             follower_pos[i].y,
        //             GRIDSIZE,
        //             GRIDSIZE
        //         };
        //         Color color = game_is_over ? RED : DARKGRAY;
        //
        //         if (i == player.score - 1) {
        //             int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);
        //
        //             for (int n = 0; n < buffer_active_count; ++n) {
        //                 int index = (tail + n) % SIZE_FILL_BLOCK;
        //                 if (CheckCollisionRecs(temp, fill_blocks[index])) {
        //                     tail = (index + 1) % SIZE_FILL_BLOCK;
        //                     break;
        //                 }
        //             }
        //         }
        //
        //         DrawRectangleRec(temp, color);
        //
        //         Vector2 wrapOffsets[4] = {
        //             { -SCREEN_WIDTH, 0 },
        //             { SCREEN_WIDTH, 0 },
        //             { 0, -SCREEN_HEIGHT },
        //             { 0, SCREEN_HEIGHT }
        //         };
        //
        //         for (int j = 0; j < 4; j++) {
        //             Vector2 offset = wrapOffsets[j];
        //             Rectangle ghost = {
        //                 temp.x + offset.x,
        //                 temp.y + offset.y,
        //                     GRIDSIZE,
        //                 GRIDSIZE
        //             };
        //             if (ghost.x + GRIDSIZE > 0 && ghost.x < SCREEN_WIDTH &&
        //                 ghost.y + GRIDSIZE > 0 && ghost.y < SCREEN_HEIGHT) {
        //                 DrawRectangleRec(ghost, color);
        //             }
        //         }
        //         if (CheckCollisionRecs(temp, eating_rect)) {
        //             score = true;
        //             spawn = true;
        //         }
        //         if (i != 0 && CheckCollisionRecs(temp, player.rect)) {
        //             // game_over(&player);
        //             // game_is_over = true;
        //         }
        //     }
        // }


        for (int i = 0; i < player.score; ++i) {
            int delay = follower_delay[i];
            if (delay < SIZE) {
                follower_pos[i] = get_history_pos(delay);

                // Create the player's current segment rectangle
                Rectangle temp = {
                    follower_pos[i].x,
                    follower_pos[i].y,
                    GRIDSIZE,
                    GRIDSIZE
                };

                Color color = game_is_over ? RED : DARKGRAY;

                // Check for collision between the snake's head and the fill blocks
                if (i == player.score - 1) {
                    int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);

                    // Check for collisions with the fill block
                    for (int n = 0; n < buffer_active_count; ++n) {
                        int index = (tail + n) % SIZE_FILL_BLOCK;
                        // Check for collision between this segment and the ghost block
                        if (CheckCollisionRecs(temp, fill_blocks[index])) {
                            tail = (index + 1) % SIZE_FILL_BLOCK;
                            break;
                        }
                    }
                }

                // Draw the player's body segment
                DrawRectangleRec(temp, color);

                // Offset for wrapping the fill block at the corners
                Vector2 wrapOffsets[4] = {
                    { -SCREEN_WIDTH, 0 },   // Left wrap
                    { SCREEN_WIDTH, 0 },    // Right wrap
                    { 0, -SCREEN_HEIGHT },  // Top wrap
                    { 0, SCREEN_HEIGHT }    // Bottom wrap
                };

                // Check if the fill block (ghost) has wrapped and check for collision
                for (int j = 0; j < 4; j++) {
                    Vector2 offset = wrapOffsets[j];
                    Rectangle ghost = {
                        temp.x + offset.x,
                        temp.y + offset.y,
                        GRIDSIZE,
                        GRIDSIZE
                    };

                    // Only draw the ghost if it is within the screen bounds
                    if (ghost.x + GRIDSIZE > 0 && ghost.x < SCREEN_WIDTH &&
                        ghost.y + GRIDSIZE > 0 && ghost.y < SCREEN_HEIGHT) {
                        DrawRectangleRec(ghost, color);

                        // Check if the wrapped ghost block collides with the snake
                        if (CheckCollisionRecs(temp, ghost)) {
                            // Handle collision (game over or similar logic)
                            // game_over(&player);
                            // game_is_over = true;
                        }
                    }
                }

            // Check collision with the eating area
            if (CheckCollisionRecs(temp, eating_rect)) {
                score = true;
                spawn = true;
            }

            // Check for self-collision (snake's head colliding with its body)
            if (i != 0 && CheckCollisionRecs(temp, player.rect)) {
                // game_over(&player);
                // game_is_over = true;
            }
        }
    }


        draw_filler();

        if (score) player.score++;
        if (spawn) eating_rect = spawn_block();
        DrawRectangleRec(eating_rect, RED);
        DrawFPS(10, 10);
        spawn = false;
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

