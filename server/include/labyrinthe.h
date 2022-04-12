#ifndef LABY
#define LABY


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXMAP 999
#define MINMAP 10
#define WIDTH 11
#define HEIGHT 11

void freeMap(int** arr, int height);
void printMap(int** arr, int height, int width);
int** updateMap(int** arr, int height, int width);
int** initMap(int height, int width);
int** fillMap(int** arr, int height, int width);
int** createMap(int height, int width);

#endif