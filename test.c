#include <stdio.h>
#include "./raylib-5.5_linux_amd64/include/raylib.h"
// #include "lib.c"


Vector2 Vector2Subtract(Vector2 curr, Vector2 prev) {
    return (Vector2){ curr.x - prev.x, curr.y - prev.y};
}


bool Vector2Equals(Vector2 curr, Vector2 prev) {
    return (curr.x == prev.x && curr.y == prev.y) ? true : false;
}



int main() {

    Vector2 pos_1 = {1.0f, 2.0f };
    Vector2 pos_2 = {2.0f, 2.0f };
    Vector2 pos_3 = {1.0f, 2.0f };

    Vector2 pos = Vector2Subtract(pos_1, pos_2);

    printf("%f, %f\n", pos_1.x, pos_1.y);
    printf("%f, %f\n", pos_2.x, pos_2.y);
    printf("%f, %f\n", pos.x, pos.y);

    if (Vector2Equals(pos_1, pos_2)) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    if (Vector2Equals(pos_1, pos_3)) {
        printf("true\n");
    } else {
        printf("false\n");
    }
}
