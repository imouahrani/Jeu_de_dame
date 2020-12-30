#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_files.h"
#include "game_sockets.h"
#include "game_login.h"
#include "common.h"

int main(int argc, char *argv[])
{
	char pseud[TAILLE_PSEUDO] = {0};
    char buffer[1024] = {0};
	
	int i, j;
	int sock;
	if( argc != 3)
	{
		printf("Erreur il n y a pas assez d'argument\nvous devez entrez : pseudo adresseIP port mot_de_passe\n");	
		exit(-1);
	}


	char* adresse = argv[1];
	char *port = argv[2];
	Move their_move;
	Board board;

	ps.csock = socket_connect( adresse, port);

	definirModeConnexionClient( ps.csock, buffer);
	ps.lock = definirCouleurClient( ps.csock, buffer);
	
	fd_set readfds;
	struct timeval interval;
	
	interval.tv_sec = 5;
	interval.tv_usec = 0;
	
	int ret = -1;
	
	if( ps.mode == 'J' )
	{
		int intCouleur = listeJoueurs( ps.csock , buffer);
		if( intCouleur == 0)
		{
			ps.lock = 'W';
		}
		else
		{
			ps.lock = 'B';
		}
		
		CreateBoard(&board);
		InitializeBoard(&board, ps.lock);
		InitializePlayer(&board, &ps, ps.lock);
	
		fprintf(stderr, "ps.lock : %c\n", ps.lock);
		do
		{
			FD_ZERO(&readfds);
			FD_SET(ps.csock, &readfds);
			
			// check every 2 seconds if there is a socket to read from
			ret = select(ps.csock + 1, &readfds, NULL, NULL, &interval);
			
			ShowBoard(&board, ps.lock);
			printf("ps.lock %c\n", ps.lock);
			if( ps.lock == 'W' )
			{
				// if FD_ISSET ps.csock
				
				// c'est aux blancs de jouer
				// MovePawn when there's something to send
				MovePawn(&board, &ps);
				ShowBoard(&board, ps.lock);
				if( FD_ISSET(ps.csock, &readfds) )
				{
					socket_recv(ps.csock, &their_move, sizeof(their_move));
					// execute this only if we have received something
					AdversaryMove(&board, &ps, &their_move);
				}

			}
			else if( ps.lock == 'B' )
			{
				// c'est aux noirs de jouer
				if( FD_ISSET(ps.csock, &readfds) )
				{
					socket_recv(ps.csock, &their_move, sizeof(their_move));
					AdversaryMove(&board, &ps, &their_move);
				}
					ShowBoard(&board, ps.lock);
					MovePawn(&board, &ps);
			}
		}while( 1 );
	}
	
	return 0;
}

