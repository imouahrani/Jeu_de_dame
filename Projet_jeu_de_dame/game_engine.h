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
	int en_attente;
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


struct Partie
{
	struct Client joueur1;
	struct Client joueur2;
	int drapeau;
	Board board;
	int nbSpectateur;
    struct Client tabSpectateur[10];

};

typedef struct MOVE
{
	Point from, to, eaten;
}Move;


int IsEven(int num);
void UpdatePoint(Point *point, int x, int y, char c);
void ShowPoint(Point *point);
char GetColor(Point *point);
void SetColor(Point *point, char color);

#endif

