#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_graphics.h"
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
	
	char *adresse = argv[1];
	char *port = argv[2];
	
	// their_move will hold information on the opponent's move
	Move their_move;
	Board board;

	ps.csock = socket_connect( adresse, port);

	definirModeConnexionClient( ps.csock, buffer);
	
	// la fonction listeJoueur se bloque jusqu'à ce qu'elle trouve le 2 eme joueur
	if( ps.mode == 'J'){
	
		int intCouleur = listeJoueurs( ps.csock , buffer);
		if( intCouleur == 0)
			ps.lock = 'W';
		else
			ps.lock = 'B';
	
		/// on va le mettre en attente jusqu'à ce que le 2 eme arrive
	
		CreateBoard(&board);
		InitializeBoard(&board, ps.lock);
		InitializePlayer(&board, &ps, ps.lock);
	
		fprintf(stderr, "ps.lock : %c\n", ps.lock);
	
		// main window for game
		GameSurface window;
	
		SetDimensions(&window.dimensions, WINDOW_WIDTH, WINDOW_HEIGHT);
	
		InitializeGameWindow(&window);
	
		Uint32 light_blue = CreateColor(&window, 163, 160, 254);
		ColorSurface(&window, light_blue);
		// refresh the window (update changes)
		Update(&window);
	
		Uint32 light_red = CreateColor(&window, 224, 220, 213);
		Uint32 light_black = CreateColor(&window, 79, 85, 94);
	
		// squares - red and black
		GameSurface red_black_rect;
	
		SetDimensions(&red_black_rect.dimensions, SQUARE_WIDTH, SQUARE_HEIGHT);
		red_black_rect.color = light_red;
	
		CreateRect(&red_black_rect);
	
		// draw the squares here
		CreateGraphicsBoard(&window, &red_black_rect, light_red, light_black);
	
		// pawns - white and black
		// pawns are in the Board structure
		// drawing of all pawns happens here
		GameSurface black_pawn;
		GameSurface white_pawn;
	
		LoadFromImage(&black_pawn, "sprites/black_piece.png", 3, 3);
		LoadFromImage(&white_pawn, "sprites/red_piece.png", 10, 10);
		// draw all black pawns on the board
		ShowGraphicsBoard(&window, &white_pawn, &black_pawn, &board, 'W');
	
		Update(&window);
	
		Move playerMove;
		int played = 0, received = 0;
	
		while( 1 )
		{
			SDL_Event event;
			if( ps.lock == 'W' )
			{
				if( SDL_PollEvent(&event) )
				{
					if( event.type == SDL_QUIT )
					{
						break;
					}
					else if( event.type == SDL_MOUSEBUTTONUP )
					{
						if( event.button.button == SDL_BUTTON_LEFT )
						{
							Point mouse_coords;
						
							// get board equivalent coordinates here
							// to send to MovePawn
							Point board_coords;
						
							SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);
							BoxClicked(&mouse_coords, &board_coords);
						
							// check index of pawn
							// if index > -1 then it's a selection
							// fill the `Point from` in the `Move` struct
							int index = FindPawnIndex(&board, &board_coords);
							if( index > - 1 )
							{
								// fill pawn structure here and access pawn coordinates
								UpdatePoint(&playerMove.from, board.pawns[index].position.x, board.pawns[index].position.y, board.pawns[index].position.color);
								ShowPoint(&playerMove.from);
							}
							else
							{
								if( playerMove.from.x > -1 && playerMove.from.y > -1 )
								{
									// if here then a pawn is selected
									// and the square clicked currently is empty
									// fill the square information in the `Point to` struct in `Move`
									// check if the move is valid
									// board coords has the `Point to` coordinates
									UpdatePoint(&playerMove.to, board_coords.x, board_coords.y, board_coords.color);
									// clear the current square
									// move the pawn here
									int success = MovePawnWithPoint(&board, &ps, &playerMove);
								
									if( success )
									{
										// remove eaten pawn, if any
										if( playerMove.eaten.x > -1 && playerMove.eaten.y > -1 )
										{
											printf("--------------DEBUGGING CLEAR WHITE SUCCESS-------------\n\n");
											ShowPoint(&their_move.eaten);
											int sq_index = FindSquareIndex(&their_move.eaten);
											printf("Square index: %d\n", sq_index);
											printf("coords\n");
											ShowPoint(&board.squares[sq_index]);
											RemovePawn(&board, sq_index);
											ShowPoint(&board.squares[sq_index]);
											printf("-------------CLEAR WHITE END-------------\n\n");
											ClearSquare(&window, &red_black_rect, &playerMove.eaten, light_red);
										}
										// update graphical board here
										ClearSquare(&window, &red_black_rect, &playerMove.from, light_red);
										// update pawn image coordinates
										SetCoordinates(&white_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
										// draw the pawn
										DrawObject(&white_pawn, &window);
										Update(&window);
									
										memset(&playerMove, -1, sizeof(Move));
										played = 1;
										received = 0;
									}
								}
							}
						}
					}
				}
				// if we've played and not received, then we receive
				else if( played && !received )
				{
					ShowBoard(&board, ps.lock);
					socket_recv(ps.csock, &their_move, sizeof(their_move));
					AdversaryMove(&board, &ps, &their_move);
					
					// any pawn that has been eaten
					// remove eaten pawn, if any
					if( their_move.eaten.x > -1 && their_move.eaten.y > -1 )
					{
						printf("--------------DEBUGGING CLEAR WHITE-------------\n\n");
						ShowPoint(&their_move.eaten);
						int sq_index = FindSquareIndex(&their_move.eaten);
						printf("Square index: %d\n", sq_index);
						printf("coords\n");
						ShowPoint(&board.squares[sq_index]);
						RemovePawn(&board, sq_index);
						ShowPoint(&board.squares[sq_index]);
						printf("-------------CLEAR WHITE END-------------\n\n");
						ClearSquare(&window, &red_black_rect, &their_move.eaten, light_red);
					}
					// update graphical board here
					ClearSquare(&window, &red_black_rect, &their_move.from, light_red);
					// update pawn image coordinates
					SetCoordinates(&black_pawn.coordinates, their_move.to.x * SQUARE_WIDTH, their_move.to.y * SQUARE_HEIGHT);
					// draw the pawn
					DrawObject(&black_pawn, &window);
					Update(&window);
					played = 0;
					received = 1;
				}
			}
			else if( ps.lock == 'B' )
			{
				// c'est aux noirs de jouer
				// we haven't played and haven't received
				if( !received )
				{
					ShowBoard(&board, ps.lock);
					socket_recv(ps.csock, &their_move, sizeof(their_move));
					fprintf(stderr, "rcvd something\n");
					AdversaryMove(&board, &ps, &their_move);
					
					// any pawn that has been eaten
					// remove eaten pawn, if any
					if( their_move.eaten.x > -1 && their_move.eaten.y > -1 )
					{
						printf("--------------DEBUGGING CLEAR BLACK-------------\n\n");
						ShowPoint(&their_move.eaten);
						int sq_index = FindSquareIndex(&their_move.eaten);
						printf("Square index: %d\n", sq_index);
						printf("coords\n");
						ShowPoint(&board.squares[sq_index]);
						RemovePawn(&board, sq_index);
						ShowPoint(&board.squares[sq_index]);
						printf("-------------CLEAR BLACK END-------------\n\n");
						ClearSquare(&window, &red_black_rect, &their_move.eaten, light_black);
					}
					ShowBoard(&board, ps.lock);
					
					// update graphical board here
					ClearSquare(&window, &red_black_rect, &their_move.from, light_black);
					// update pawn image coordinates
					SetCoordinates(&white_pawn.coordinates, their_move.to.x * SQUARE_WIDTH, their_move.to.y * SQUARE_HEIGHT);
					// draw the pawn
					DrawObject(&white_pawn, &window);
					Update(&window);
				
					received = 1;
				}
				if( SDL_PollEvent(&event) )
				{
					if( event.type == SDL_QUIT )
					{
						fprintf(stderr, "rcvd quit\n");
						break;
					}
					else if( event.type == SDL_MOUSEBUTTONUP )
					{
						fprintf(stderr, "button down\n");
						if( event.button.button == SDL_BUTTON_LEFT )
						{
							Point mouse_coords;
						
							// get board equivalent coordinates here
							// to send to MovePawn
							Point board_coords;
						
							SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);
							BoxClicked(&mouse_coords, &board_coords);
						
							// check index of pawn
							// if index > -1 then it's a selection
							// fill the `Point from` in the `Move` struct
							int index = FindPawnIndex(&board, &board_coords);
							if( index > - 1 )
							{
								// fill pawn structure here and access pawn coordinates
								UpdatePoint(&playerMove.from, board.pawns[index].position.x, board.pawns[index].position.y, board.pawns[index].position.color);
								ShowPoint(&playerMove.from);
							}
							else
							{
								if( playerMove.from.x > -1 && playerMove.from.y > -1 )
								{
									// if here then a pawn is selected
									// and the square clicked currently is empty
									// fill the square information in the `Point to` struct in `Move`
									// check if the move is valid
									// board coords has the `Point to` coordinates
									UpdatePoint(&playerMove.to, board_coords.x, board_coords.y, board_coords.color);
									// clear the current square
									// move the pawn here
									int success = MovePawnWithPoint(&board, &ps, &playerMove);
								
									if( success )
									{
										// remove eaten pawn, if any
										if( playerMove.eaten.x > -1 && playerMove.eaten.y > -1 )
										{
											printf("--------------DEBUGGING CLEAR BLACK SUCCESS-------------\n\n");
											ShowPoint(&their_move.eaten);
											int sq_index = FindSquareIndex(&their_move.eaten);
											printf("Square index: %d\n", sq_index);
											printf("coords\n");
											ShowPoint(&board.squares[sq_index]);
											RemovePawn(&board, sq_index);
											ShowPoint(&board.squares[sq_index]);
											printf("-------------CLEAR BLACK END-------------\n\n");
											ClearSquare(&window, &red_black_rect, &playerMove.eaten, light_black);
										}
										// update graphical board here
										ClearSquare(&window, &red_black_rect, &playerMove.from, light_black);
										// update pawn image coordinates
										SetCoordinates(&black_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
										// draw the pawn
										DrawObject(&black_pawn, &window);
										Update(&window);
									
										memset(&playerMove, -1, sizeof(Move));
										played = 1;
										received = 0;
									}
								}
							}
						}
					}
				}
				else if( played )
				{
					played = 0;
				}
			}
		}
	
	}else{
		
		//le client est en mode spectateur
		//printf("desole on a pas encore fait cette partie :(\n");
		/// on va le mettre en attente jusqu'à ce que le 2 eme arrive
	
		CreateBoard(&board);
		InitializeBoard(&board, 'W');
		InitializePlayer(&board, &ps, 'W');
	
		fprintf(stderr, "ps.lock : %c\n", ps.lock);
	
		// main window for game
		GameSurface window;
	
		SetDimensions(&window.dimensions, WINDOW_WIDTH, WINDOW_HEIGHT);
	
		InitializeGameWindow(&window);
	
		Uint32 light_blue = CreateColor(&window, 163, 160, 254);
		ColorSurface(&window, light_blue);
		// refresh the window (update changes)
		Update(&window);
	
		Uint32 light_red = CreateColor(&window, 209, 169, 193);
		Uint32 light_black = CreateColor(&window, 66, 17, 47);
	
		// squares - red and black
		GameSurface red_black_rect;
	
		SetDimensions(&red_black_rect.dimensions, SQUARE_WIDTH, SQUARE_HEIGHT);
		red_black_rect.color = light_red;
	
		CreateRect(&red_black_rect);
	
		// draw the squares here
		CreateGraphicsBoard(&window, &red_black_rect, light_red, light_black);
	
		// pawns - white and black
		// pawns are in the Board structure
		// drawing of all pawns happens here
		GameSurface black_pawn;
		GameSurface white_pawn;
	
		LoadFromImage(&black_pawn, "sprites/black_piece.png", 3, 3);
		LoadFromImage(&white_pawn, "sprites/red_piece.png", 10, 10);
		// draw all black pawns on the board
		ShowGraphicsBoard(&window, &white_pawn, &black_pawn, &board, 'W');
	
		Update(&window);
	
		Move playerMove;
		ps.lock = 'B';
		
		while(1){

			SDL_Event event;
			if( SDL_PollEvent(&event) )
			{
				if( event.type == SDL_QUIT )
				{
					break;
				}
			}
			
			ShowBoard(&board, 'W');
			socket_recv(ps.csock, &playerMove, sizeof(playerMove));
			

			ShowPoint(&playerMove.from);
			ShowPoint(&playerMove.to);
			spectateurMove( &board, &playerMove);
			char color = GetColor(&playerMove.to);
			// update graphical board here
			ClearSquare(&window, &red_black_rect, &playerMove.from, light_red);
			// remove eaten pawn, if any
			if( playerMove.eaten.x > -1 && playerMove.eaten.y > -1 )
			{
				printf("--------------DEBUGGING CLEAR BLACK SUCCESS-------------\n\n");
				ShowPoint(&their_move.eaten);
				int sq_index = FindSquareIndex(&their_move.eaten);
				printf("Square index: %d\n", sq_index);
				printf("coords\n");
				ShowPoint(&board.squares[sq_index]);
				RemovePawn(&board, sq_index);
				ShowPoint(&board.squares[sq_index]);
				printf("-------------CLEAR BLACK END-------------\n\n");
				ClearSquare(&window, &red_black_rect, &playerMove.eaten, light_red);
			}
			if( color == 'W'){
				// update pawn image coordinates
				SetCoordinates(&white_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
				// draw the pawn
				DrawObject(&white_pawn, &window);
			
			}else if( color == 'B'){
			
				// update graphical board here

				// update pawn image coordinates
				SetCoordinates(&black_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
				// draw the pawn
				DrawObject(&black_pawn, &window);
			
			}
			
			Update(&window);				
		}
	}
	
	return 0;
}

