#ifndef LABY
#define LABY


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define MAXMAP 999
#define MINMAP 10
#define WIDTH 11
#define HEIGHT 11
#define ERROR(func_error, type_error) {                            \
            char result [strlen(func_error)+strlen(type_error)+2]; \
            sprintf(result, "%s: %s", func_error, type_error);     \
            fprintf(stderr, "%s\n", result);                       \
            return -1;                                             \
        }

void freeMap(int** arr, int height);
void printMap(int** arr, int height, int width);
int** updateMap(int** arr, int height, int width);
int** initMap(int height, int width);
int** fillMap(int** arr, int height, int width);
int** createMap(int height, int width);

#endif