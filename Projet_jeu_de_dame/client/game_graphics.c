#include "game_graphics.h"
#include "game_sockets.h"



void spectateurMove( Board *b, Move *playerMove){

	int pawn_index = -1;
	Point *from = &playerMove->from, *to = &playerMove->to, *to_remove = &playerMove->eaten;
	
	// valid move will fill to_remove with positive coordinates and a color
	// if that's the case,then there is a pawn that has been eaten

	pawn_index = FindPawnIndex(b, from);
	
	if( pawn_index < 0 )
	{
		return ;
	}
	
	fprintf(stderr, "pawn_index: %d\n", pawn_index);
	
	fprintf(stderr, "moving pawn %d from (%d, %d) to (%d, %d)\n", pawn_index, from->x, from->y, to->x, to->y);
	int square_src = FindSquareIndex(from);
	int square_dest = FindSquareIndex(to);
	Pawn *to_move = &b->pawns[pawn_index];
	
	// update pawn
	printf("color of pawn is %c\n", GetColor(&to_move->position));
	printf("index of pawn is %d\n", pawn_index);
	UpdatePoint(&to_move->position, to->x, to->y, GetColor(&to_move->position));
	
	// update squares source and destination
	SetColor(&b->squares[square_src], ' ');
	SetColor(&b->squares[square_dest], GetColor(&to_move->position));
	ShowPoint(&to_move->position);
	
	

}


void SetCoordinates(SDL_Rect *coords, int x, int y)
{
	coords->x = x;
	coords->y = y;
	
	return;
}

void GetCoordinates(SDL_Rect *coords, int *x, int *y)
{
	*x = coords->x;
	*y = coords->y;
	
	return;
}

void SetDimensions(Dimensions *dim, int w, int h)
{
	dim->width = w;
	dim->height = h;
	
	return;
}

void GetDimensions(Dimensions *dim, int *w, int *h)
{
	*w = dim->width;
	*h = dim->height;
	
	return;
}

void InitializeGameWindow(GameSurface *gs)
{
	// initialize SDL video mode
	SDL_Init(SDL_INIT_VIDEO);
	
	// window to return
	SDL_Surface *ret = SDL_SetVideoMode(gs->dimensions.width, gs->dimensions.height, 32, SDL_HWSURFACE);
	if( !ret )
	{
		fprintf(stderr, "error: %s\n", SDL_GetError());
		exit(-1);
	}
	
	// window color
	SDL_FillRect(ret, NULL, SDL_MapRGB(ret->format, 0, 0, 0));
	
	gs->surface = ret;
	
	return;
}

Uint32 CreateColor(GameSurface *gs, int r, int g, int b)
{
	gs->r = r;
	gs->g = g;
	gs->b = b;
	
	return SDL_MapRGB(gs->surface->format, gs->r, gs->g, gs->b);
}

void ColorSurface(GameSurface *gs, Uint32 color)
{
	SDL_FillRect(gs->surface, NULL, color);
}

void CreateRect(GameSurface *gs)
{
	SDL_Surface *ret = SDL_CreateRGBSurface(0, gs->dimensions.width, gs->dimensions.height, 32, 0, 0, 0, 0);
	
	gs->surface = ret;
	// color the rectangle
	ColorSurface(gs, gs->color);
	
	return;
}

// object to draw in src
// where to draw in dest
void DrawObject(GameSurface *src, GameSurface *dest)
{
	SDL_BlitSurface(src->surface, NULL, dest->surface, &src->coordinates);
	
	return;
}

void LoadFromImage(GameSurface *gs, const char *img, int x_factor, int y_factor)
{
	gs->surface = IMG_Load(img);
	
	ResizeImage(gs, x_factor, y_factor);
}

void ResizeImage(GameSurface *img, int x_factor, int y_factor)
{
	// CODE TO REDUCE SIZE OF PAWNS TO FIT SQUARES
	SDL_PixelFormat *pixelFmt = img->surface->format;
	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, img->surface->w / x_factor, img->surface->h / y_factor, pixelFmt->BitsPerPixel, pixelFmt->Rmask, pixelFmt->Gmask, pixelFmt->Bmask, pixelFmt->Amask);
	
	SDL_FillRect(tmp, NULL, SDL_MapRGBA(tmp->format, 0, 0, 0, 0));
	SDL_SoftStretch(img->surface, NULL, tmp, NULL);
	
	SDL_FreeSurface(img->surface);
	
	img->surface = tmp;
	
	return;
}

void Update(GameSurface *gs)
{
	SDL_Flip(gs->surface);
	
	return;
}

// board creation and initialization graphics functions
void CreateGraphicsBoard(GameSurface *window, GameSurface *square, Uint32 color1, Uint32 color2)
{
	int i = 0, j = 0;
	
	for( i = 0; i < NUM_SQUARES; ++i )
	{
		for( j = 0; j < NUM_SQUARES; ++j )
		{
			SetCoordinates(&square->coordinates, (j * SQUARE_WIDTH), (i * SQUARE_HEIGHT));
			
			if( !IsEven(i) )
			{
				if( IsEven(j) )
				{
					ColorSurface(square, color1);
				}
				else
				{
					ColorSurface(square, color2);
				}
			}
			else
			{
				if( !IsEven(j) )
				{
					ColorSurface(square, color1);
				}
				else
				{
					ColorSurface(square, color2);
				}
			}
			
			DrawObject(square, window);
		}
	}
	
	return;
}

void ShowGraphicsBoard(GameSurface *window, GameSurface *white_pawns, GameSurface *black_pawns, Board *b, char color)
{
	int i = 0;
	
	for( i = 0; i < BOARD_SIZE; ++i )
	{
		if( b->squares[i].color == 'W' )
		{
			SetCoordinates(&white_pawns->coordinates, b->squares[i].x * SQUARE_WIDTH, b->squares[i].y * SQUARE_HEIGHT);
			DrawObject(white_pawns, window);
		}
		else if( b->squares[i].color == 'B' )
		{
			SetCoordinates(&black_pawns->coordinates, b->squares[i].x * SQUARE_WIDTH, b->squares[i].y * SQUARE_HEIGHT);
			DrawObject(black_pawns, window);
		}
	}
	
	return;
}

void ClearSquare(GameSurface *window, GameSurface *square, Point *coordinates, Uint32 color)
{
	// multiply by SQUARE_WIDTH and height the index in x and y
	int x_coord = coordinates->x * SQUARE_WIDTH;
	int y_coord = coordinates->y * SQUARE_HEIGHT;
	
	SetCoordinates(&square->coordinates, x_coord, y_coord);
	ColorSurface(square, color);
	
	// draw the square on the window
	DrawObject(square, window);
	Update(window);
	
	return;
}

void BoxClicked(Point *click_coords, Point *out_coords)
{
	int x = click_coords->x / SQUARE_WIDTH;
	int y = click_coords->y / SQUARE_HEIGHT;
	
	UpdatePoint(out_coords, x, y, ' ');
	
	return;
}

int MovePawnWithPoint(Board *b, Player *p, Move *playerMove)
{
	int pawn_index = -1;
	char lock = p->lock;
	Point *from = &playerMove->from, *to = &playerMove->to, *to_remove = &playerMove->eaten;
	
	// valid move will fill to_remove with positive coordinates and a color
	// if that's the case,then there is a pawn that has been eaten
	pawn_index = ValidMove(b, p, from, to, to_remove);
	if( pawn_index < 0 )
	{
		return 0;
	}
	
	fprintf(stderr, "pawn_index: %d\n", pawn_index);
	
	fprintf(stderr, "moving pawn %d from (%d, %d) to (%d, %d)\n", pawn_index, from->x, from->y, to->x, to->y);
	int square_src = FindSquareIndex(from);
	int square_dest = FindSquareIndex(to);
	Pawn *to_move = &b->pawns[pawn_index];
	
	// update pawn
	printf("color of pawn is %c\n", GetColor(&to_move->position));
	printf("index of pawn is %d\n", pawn_index);
	UpdatePoint(&to_move->position, to->x, to->y, GetColor(&to_move->position));
	
	// update squares source and destination
	SetColor(&b->squares[square_src], ' ');
	SetColor(&b->squares[square_dest], GetColor(&to_move->position));
	ShowPoint(&to_move->position);
	
	// si il arrive ici, alors on envoie
	Move to_send;
	
	UpdatePoint(&to_send.from, from->x, from->y, GetColor(&b->squares[square_src]));
	UpdatePoint(&to_send.to, to->x, to->y, GetColor(&to_move->position));
	UpdatePoint(&to_send.eaten, to_remove->x, to_remove->y, GetColor(to_remove));
	
	printf("----------------DEBUGGING indices in move pawn with point-----------\n\n");
	ShowPoint(&to_send.from);
	ShowPoint(&to_send.to);
	ShowPoint(&to_send.eaten);
	printf("----------------end debug indices in move pawn with point ------------\n\n");
	
	socket_send(p->csock, &to_send, sizeof(to_send));
	
	return 1;
}

