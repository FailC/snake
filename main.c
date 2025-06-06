// #include <stdio.h>
#include "./lib.c"


// include into snake struct?
static int head = 0;
static int tail = 0;


int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "solid snake!!");
    SetTargetFPS(60);
    // SetTargetFPS(5);

    // player
    Rectangle p_rect = { RECT_WIDTH, RECT_WIDTH, GRIDSIZE, GRIDSIZE };
    Player player = {p_rect, .score = 0, SPEED};

    Game game = { .highscore = 0 };

    game.highscore = load_file();
    // if (!game.highscore) {
    //     game.highscore = 0;
    // }

    Rectangle eating_rect = {};

    Snake snake = {};

    for (int i = 0; i < FOLLOWER; ++i) {
        int step_size = GRIDSIZE / SPEED;
        snake.body.delay[i] = step_size * (i + 1);
    }

    int rectSize = player.rect.height;
    bool score = false;
    bool spawn_eating_rect = true;

    // bool game_is_over = false;
    // Rectangle fill_blocks[SIZE_FILL_BLOCK];

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
        save_pos(&snake.history, (Vector2){ player.rect.x, player.rect.y });

        if (game.is_over && IsKeyPressed(KEY_SPACE)) {
            game_restart(&player, &snake.history, snake.fill_blocks, &head, &tail);
            score = false;
            spawn_eating_rect = true;
            game.is_over = false;
        }

        // printf("player-> %f : %f\n", player.rect.x, player.rect.y);

        if (CheckCollisionRecs(player.rect, eating_rect)) {
            spawn_eating_rect = true;
            score = true;
        }

        // important function, stores the fill blocks
        if (player.score >= 1) {
            if (direction_change(&snake.history)) {
                // printf("direction changed\n");
                insert_fill_block(&snake.history, snake.fill_blocks, &head);
            }
            // printf("head: %d\n", head);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!game.is_over) {
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
                // DrawRectangleRec(ghost, GREEN);
                DrawRectangleRec(ghost, BLACK);

                if (CheckCollisionRecs(ghost, eating_rect)) {
                    spawn_eating_rect = true;
                    score = true;
                }
            }
        }

        // draw follow blocks
        // for the last index, check collision with fill_blocks

        for (int i = 0; i < player.score; ++i) {
            // int delay = follower_delay[i];
            int delay = snake.body.delay[i];
            if (delay < HISTORY_SIZE) {
                // follower_pos[i] = get_prev_pos(&history,delay);
                snake.body.pos[i] = get_prev_pos(&snake.history, delay);

                Rectangle temp = {
                    snake.body.pos[i].x,
                    snake.body.pos[i].y,
                    GRIDSIZE,
                    GRIDSIZE
                };
                Color color = game.is_over ? RED : DARKGRAY;

                if (i == player.score - 1) {
                    int buffer_active_count = (head >= tail) ? (head - tail) : (SIZE_FILL_BLOCK - tail + head);
                    // printf("active: %d\n", buffer_active_count);

                    for (int n = 0; n < buffer_active_count; ++n) {
                        int index = (tail + n) % SIZE_FILL_BLOCK;
                        // if (CheckCollisionRecs(temp, fill_blocks[index])) {
                        // Rectangle area = GetCollisionRec(temp, fill_blocks[index]);
                        Rectangle area = GetCollisionRec(temp, snake.fill_blocks[index]);
                        if (area.height == temp.height && area.width == temp.width) {
                            tail = (index + 1) % SIZE_FILL_BLOCK;
                        }
                        break;
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
                    game_over(&player, &game.highscore);
                    game.is_over = true;
                }
            }
        }

        // draw fill blocks
        draw_filler(snake.fill_blocks, &tail, &head, game.is_over);

        if (score && !game.is_over) player.score++;
        if (spawn_eating_rect) eating_rect = spawn_block();
        DrawRectangleRec(eating_rect, RED);
        DrawFPS(10, 10);
        spawn_eating_rect = false;
        score = false;

        if (game.is_over) {
            DrawText("GAME OVER!", 100, 100, 20, BLACK);
            DrawText("PRESS SPACE TO RESTART!", 100, 150, 20, BLACK);
        }

        draw_int_to_text(player.score * 100, 150, 10);
        draw_int_to_text(game.highscore * 100, 500, 10);
        DrawText("Highscore", 350, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

