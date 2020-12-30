#ifndef _GAME_ENGINE_H
#define _GAME_ENGINE_H

#define	BUFFSIZE 200
#define	SOCKET_ERROR -1	
#define MAX_CLIENT   30
#define TAILLE_PSEUDO 30
#define TAILLE_MOT_DE_PASSE 30
#define NB_UTILISATEURS 10

#define BOARD_SIZE			64
#define NUM_PAWNS			24
#define NUM_SQUARES			8

struct Client 
{
	char pseudo[TAILLE_PSEUDO];
	char motDePasse[TAILLE_MOT_DE_PASSE];
	int csock;	
	char mode; // J joueur S spectateur (maj)
	char couleur;
};


typedef struct POINT
{
	int x, y;
	char color;
}Point;

typedef struct PAWN
{
	Point position;
}Pawn;

typedef struct BOARD
{
	Point squares[BOARD_SIZE];
	Pawn *pawns;
	int num_pawns;
}Board;

/*struct Partie
{
	struct Client joueur1;
	struct Client joueur2;
	int drapeau;
	Board board;

};
*/

typedef struct MOVE
{
	Point from, to, eaten;
}Move;

typedef struct PLAYER
{
	int csock;
	Pawn *pawns;			// can these pawns reference the board pawns ???
	int num_pawns;
	char lock;
	char pseudo[TAILLE_PSEUDO];
	char motDePasse[TAILLE_MOT_DE_PASSE];
	char mode; // J joueur S spectateur (maj)
}Player;

int IsEven(int num);
void CreateBoard(Board *b);
void InitializeBoard(Board *b, char color);
void InitializePlayer(Board *board, Player *player, char color);
void UpdatePoint(Point *point, int x, int y, char c);
void ShowPoint(Point *point);
char GetColor(Point *point);
void SetColor(Point *point, char color);
int AdversaryMove(Board *b, Player *p, Move *m);
// given board and point, find pawn index
// later used to find index using mouse click coordinates
int FindPawnIndex(Board *b, Point *square);
// to saves the distance between the points
Point CalculateDistance(Point *from, Point *to);
// find square index given x and y coordinate
int FindSquareIndex(Point *square);
void RemovePawn(Board *b, int square_index);
int ValidMove(Board *b, Player *p, Point *from, Point *to, Point *to_remove);
void ShowBoard(Board *b, char color);
int MovePawn(Board *b, Player *p);

// me est une variable globale
extern Player ps;

#endif

