#include "./raylib-5.5_linux_amd64/include/raylib.h"

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 500
#define GRIDSIZE 50
#define HISTORY_SIZE 2500
#define SIZE_FILL_BLOCK 50
#define SPEED 5.0f

#define FOLLOWER 250

#define RECT_WIDTH 100

typedef struct {
    Rectangle rect;
    int score;
    float PLAYER_SPEED;
} Player;

typedef struct {
    Vector2 positions[HISTORY_SIZE];
    int index;
} Pos_history;

typedef struct {
    Vector2 pos[FOLLOWER];
    int delay[FOLLOWER];
} Body;


typedef struct {
    Pos_history history;
    Body body;
    Rectangle fill_blocks[SIZE_FILL_BLOCK];
} Snake;

typedef struct {
    int highscore;
    bool is_over;
} Game;


int load_file(void);
void game_over(Player *player, int *highscore);
void game_restart(Player *player, Pos_history *history, Rectangle fill_blocks[], int *head, int *tail);
bool direction_change(const Pos_history *history);
void save_pos(Pos_history *history,Vector2 const pos);
Vector2 get_history_pos(Pos_history *history, const int steps_back);
Vector2 get_prev_pos(const  Pos_history*history, const int steps_back);
void draw_int_to_text(const int element, const int posX, const int posY);
void move_player(Player *player);
void wrap_player(Player *player);
int GetRandomDivisible(const int divisor, const int min, const int max);
Rectangle spawn_block(void);
void insert_fill_block(const Pos_history *history, Rectangle fill_blocks[], int *head);
void draw_filler(Rectangle fill_blocks[], int *tail_p, int *head_p, bool game_is_over);
