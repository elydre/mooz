/*
generate labyrinth with this format:

1 1 1 1 1 1 1 1
1 0 0 0 0 0 0 1
1 0 0 0 0 0 0 1
1 0 0 1 1 1 1 1
1 0 0 1 0 0 0 1
1 0 0 1 0 0 0 1
1 0 0 1 1 0 0 1
1 0 0 0 0 0 0 1
1 0 0 0 0 0 0 1
1 1 1 1 1 1 1 1

1 - wall
0 - empty space

corridors should be at least 2 cells wide

*/

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 20
#define HEIGHT 20

#define WALL 1
#define EMPTY 0

void divide_labyrinth(int *labyrinth, int x, int y, int width, int height);
void generate_labyrinth(int *labyrinth, int width, int height);

void print_labyrinth(int *labyrinth) {
    for (int y = 0; y < HEIGHT; y++)     {
        for (int x = 0; x < WIDTH; x++)
            printf("%d ", labyrinth[y * WIDTH + x]);
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int *labyrinth = malloc(sizeof(int) * HEIGHT * WIDTH);

    generate_labyrinth(labyrinth, WIDTH, HEIGHT);

    print_labyrinth(labyrinth);

    return 0;
}

void generate_labyrinth(int *labyrinth, int width, int height) {
    // Set all cells as walls initially
    for (int i = 0; i < width * height; i++) {
        labyrinth[i] = WALL;
    }

    // Start recursive division to create the labyrinth
    // divide_labyrinth(labyrinth, 1, 1, width - 2, height - 2);
}
