#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_sockets.h"

int IsEven(int num)
{
	return num % 2 == 0;
}

void UpdatePoint(Point *point, int x, int y, char c)
{
	point->x = x;
	point->y = y;
	point->color = c;
	
	return;
}

void ShowPoint(Point *point)
{
	fprintf(stderr, "(%d, %d, %c)\n", point->x, point->y, point->color);
}

char GetColor(Point *point)
{
	return point->color;
}

void SetColor(Point *point, char color)
{
	point->color = color;
	
	return;
}

