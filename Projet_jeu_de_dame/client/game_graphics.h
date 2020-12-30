#ifndef _GAME_GRAPHICS_H
#define _GAME_GRAPHICS_H

#include <SDL.h>
#include <SDL_image.h>

#include "game_engine.h"

#define WINDOW_WIDTH				560
#define WINDOW_HEIGHT				560
#define SQUARE_WIDTH				70
#define SQUARE_HEIGHT				70

typedef struct DIMENSIONS
{
	int width, height;
}Dimensions;

typedef struct GAMESURFACE
{
	SDL_Surface *surface;
	Uint32 color;
	int r, g, b;
	Dimensions dimensions;
	SDL_Rect coordinates;
}GameSurface;

typedef struct SQUARESELECTOR
{
	int square_selected;
}SquareSelector;

void SetCoordinates(SDL_Rect *coords, int x, int y);
void GetCoordinates(SDL_Rect *coords, int *x, int *y);
void SetDimensions(Dimensions *dim, int w, int h);
void GetDimensions(Dimensions *dim, int *w, int *h);
void InitializeGameWindow(GameSurface *gs);
Uint32 CreateColor(GameSurface *gs, int r, int g, int b);
void ColorSurface(GameSurface *gs, Uint32 color);
void CreateRect(GameSurface *gs);
void DrawObject(GameSurface *src, GameSurface *dest);
void LoadFromImage(GameSurface *gs, const char *img, int x_factor, int y_factor);
void ResizeImage(GameSurface *img, int x_factor, int y_factor);
void Update(GameSurface *gs);
void CreateGraphicsBoard(GameSurface *window, GameSurface *square, Uint32 color1, Uint32 color2);
void ShowGraphicsBoard(GameSurface *window, GameSurface *white_pawns, GameSurface *black_pawns, Board *b, char color);
void spectateurMove( Board *b, Move *playerMove);
// gives back the index in the board
// value stored in out_coords
void BoxClicked(Point *click_coords, Point *out_coords);
// clears a square and puts the color to empty
void ClearSquare(GameSurface *window, GameSurface *gs, Point *coords, Uint32 color);
int MovePawnWithPoint(Board *b, Player *p, Move *playerMove);

#endif

