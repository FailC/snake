#include "./raylib-5.5_linux_amd64/include/raylib.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 500
#define GRIDSIZE 50
#define HISTORY_SIZE 2500
#define SIZE_FILL_BLOCK 50
#define SPEED 5.0f

typedef struct {
    Rectangle rect;
    int score;
    float PLAYER_SPEED;
} Player;

typedef struct {
    Vector2 positions[HISTORY_SIZE];
    int index;
} posHistory;

int load_file(void);
void game_over(Player *player, int *highscore);
void game_restart(Player *player, posHistory *history, Rectangle fill_blocks[], int *head, int *tail);
bool direction_change(const posHistory *history);
void save_pos(posHistory *history,Vector2 const pos);
Vector2 get_history_pos(posHistory *history, const int steps_back);
Vector2 get_prev_pos(const posHistory *history, const int steps_back);
void draw_int_to_text(const int element, const int posX, const int posY);
void move_player(Player *player);
void wrap_player(Player *player);
int GetRandomDivisible(const int divisor, const int min, const int max);
Rectangle spawn_block(void);
void insert_fill_block(Player *player, const posHistory *history, Rectangle fill_blocks[], int *head);
void draw_filler(Rectangle fill_blocks[], int *tail_p, int *head_p);
