#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_sockets.h"
#include "game_login.h"
#include "game_files.h"
#include "common.h"

int main(int argc, char *argv[])
{
	if( argc != 2)
	{
		printf("erreur il n'y a pas assez d'argument, vous devez entrez le port\n");	
		
		exit(-1);
	}
	
	int port = atoi( argv[1] );
	printf("\n\t\t****************************\n");
	printf("\t\t*                          *\n");
	printf("\t\t*     WELCOME TO SERVER    *\n");
	printf("\t\t*                          *\n");
	printf("\t\t****************************\n\n");
    printf("\n");

	// allocation pour chaque pseudo 
	int i, j;
	
	allocation_tab_pseudo( tab_pseudo, tab_motDepasse);
	
	/*-----------------------------------------------*/
    
	FILE *mon_flux = ouvrirFichier("gameDataBase.txt");
	// recuperer les joueurs du fichier
 	recupererJoueurs( mon_flux, tab_pseudo, tab_motDepasse, &nbJoueurActuel);
 	fclose(mon_flux);


	

	struct Client tab_client[MAX_CLIENT];
	struct Partie tab_partie[1];
	
	int nb_c = 0; // c'est le nombre de joueur actuel en mode J(joueur) ou S(spectateur)
	int nb_joueur = 0;;// seulement en mode J
	
	char buffer[BUFFSIZE] = {0};
	char newTb[BUFFSIZE] = {0};

	
	
	int selec;
	int sock = listen_socket(port);
	printf("\t\t\tEcoute demarre\n");
	fd_set readfds; 
	int max_fd = sock;
	int rec;
    
	// pour la connexion 
	
	while(1){
		
		FD_ZERO(&readfds);    			
		FD_SET(STDIN_FILENO, &readfds); 
		FD_SET(sock, &readfds); 
		
		for(i=0; i < nb_joueur/2; i++) {
		
			int nbSpectateur = 	tab_partie[i].nbSpectateur;
			FD_SET(tab_partie[i].joueur1.csock, &readfds); 
			FD_SET(tab_partie[i].joueur2.csock, &readfds); 
			
			for(j = 0; j < nbSpectateur; j++){
				 FD_SET( tab_partie[i].tabSpectateur[j].csock, &readfds);
			}	
		}	
		
		
		selec = select( max_fd + 1, &readfds, NULL, NULL, NULL); 
	
		if (selec == -1)
		{
			printf("erreur de select\n");
			exit(-1);
		}
		
		// ajoue dans le readfds
		
		if(FD_ISSET(sock , &readfds))
		{
			if(nb_c < MAX_CLIENT){
			
				struct Client nv_client = add_client(sock, &nb_c, &max_fd, mon_flux);
		
					// juste si le client a été ajouté on l'ajoute dans le tab_client
					if( nv_client.csock > -1 ){
				
						tab_client[nb_c - 1] = nv_client;
						
						// s'il est en mode joueur alors 
						if( nv_client.mode == 'J'){
							
							nb_joueur++;// on augmente le nombre de joueurs 
							
							//envoyer la liste des joueur en ligne 
							if(nb_joueur%2 != 0){
								
								send_client(nv_client.csock, "0" );		

													
							}else{
									memset(buffer, 0, BUFFSIZE);

									snprintf(buffer, BUFFSIZE, "\t%d. %s \n", 1, tab_client[nb_joueur-2].pseudo);
									send_client(nv_client.csock, buffer );
									//recevoir avec quel joueur il veut jouer
									memset(buffer, 0, BUFFSIZE);
									recv_client(nv_client.csock, buffer);


							}
							
							
							if( nb_joueur%2 != 0){
								// je dois creer une partie
								struct Partie p ;
								p.joueur1 = nv_client;
								p.drapeau = 0;	
								tab_partie[ ((nb_joueur+1)/2)-1] = p;	
								
							
							}else{
							
								tab_partie[ (nb_joueur/2)-1].joueur2 = nv_client;
								tab_partie[ (nb_joueur/2)-1].drapeau = 1;
								
								//j'envoie au client pour commencer (1 pour dire partie commencer)
								
								snprintf(buffer, BUFFSIZE, "\n\t** Partie Commencée avec %s **\n", tab_partie[ (nb_joueur/2)-1].joueur1.pseudo );
								send_client( tab_partie[ (nb_joueur/2)-1].joueur2.csock   , buffer );
								
								snprintf(buffer, BUFFSIZE, "\n\t** Partie Commencée avec %s **\n", tab_partie[ (nb_joueur/2)-1].joueur2.pseudo );
								send_client( tab_partie[ (nb_joueur/2)-1].joueur1.csock   , buffer );
								tab_partie[(nb_joueur/2)-1].nbSpectateur = 0;
								
							
							}
						// partie spectateur	
						}else if(nv_client.mode == 'S'){

							int nb_partie = nb_joueur/2;						
							//il n y a pas de partie disponible (en ligne)
							if( nb_partie == 0){
								
								send_client(nv_client.csock, "O" );			// o ici pour dire il y a pas de partie
								close(nv_client.csock);
								nb_c--;
													
							}else{
							
								for(i = 0; i < nb_partie; i++){
								
									memset(buffer, 0, BUFFSIZE);
									snprintf(buffer, BUFFSIZE, "\t%d. %s VS %s\n", i, tab_partie[i].joueur1.pseudo, tab_partie[i].joueur2.pseudo  );
																	
									send_client(nv_client.csock, buffer );
								}
									//recevoire quelle partie veut regarder
									memset(buffer, 0, BUFFSIZE);
									recv_client(nv_client.csock, buffer);
									printf("il veut voir la partie num : %s\n", buffer);
									
									// on affecte dans une partie
									tab_partie[atoi(buffer)].tabSpectateur[ tab_partie[atoi(buffer)].nbSpectateur] = nv_client;

									tab_partie[atoi(buffer)].nbSpectateur++ ;

							}						
						}
					}
							
				}
		}
			

		
	
		// si on a une connexion d'une des sockets qui se trouve dans le readfds
		
		for(i = 0; i < nb_joueur/2; i++)
		{

			if( FD_ISSET( tab_partie[i].joueur1.csock, &readfds))
			{
			
				Move coordonne;
				memset(&coordonne, -1, sizeof( coordonne) );	
				
				memset(buffer, 0, BUFFSIZE);
				memset(newTb, 0, BUFFSIZE);
				
				rec = socket_recv( tab_partie[i].joueur1.csock , &coordonne , sizeof(coordonne));

				if(rec==0)
				{

					// juste si on veut informer les autres qu'un joueur est parti
					/*for(j = 0; j < nb_c; j++)
					{
						if( j != i)
						{
						
                            memset(quitte, 0, BUFFSIZE);
						    strcat(quitte, "Serveur : ");
						    strcat(quitte,tab_client[i].pseudo);
				            strcat(quitte," a quitte le jeu :(\n");
							send_client(tab_client[j].csock, quitte);
						}
					}
					*/
					rmv_client(tab_client, i, &nb_c);

					
				}else{

					// on envoie aussi au spectateur					
					if( tab_partie[i].nbSpectateur != 0 ){
					
						for( j=0; j< tab_partie[i].nbSpectateur; j++){
						
							socket_send( tab_partie[i].tabSpectateur[j].csock , &coordonne, sizeof(coordonne));						
						}

					}	
					
					UpdatePoint( &coordonne.from, coordonne.from.x, 7 - coordonne.from.y, coordonne.from.color );
					UpdatePoint( &coordonne.to, coordonne.to.x, 7 - coordonne.to.y, coordonne.to.color );

					printf("----------------DEBUGGING indices player1-----------\n\n");
					ShowPoint(&coordonne.from);
					ShowPoint(&coordonne.to);
					ShowPoint(&coordonne.eaten);
					printf("----------------end debug indices player1------------\n\n");
					// translate only if there is a pawn to eat
					if( coordonne.eaten.x > -1 && coordonne.eaten.y > -1 )
					{
						UpdatePoint(&coordonne.eaten, coordonne.eaten.x, 7 - coordonne.eaten.y, coordonne.eaten.color);
					}

					printf("----------------DEBUGGING indices updated player1-----------\n\n");
					ShowPoint(&coordonne.from);
					ShowPoint(&coordonne.to);
					ShowPoint(&coordonne.eaten);
					printf("----------------end debug indices updated player1------------\n\n");
					socket_send( tab_partie[i].joueur2.csock , &coordonne, sizeof(coordonne));


					
					
				}
				
			}
			// pour l'autre joueur
			else if( FD_ISSET( tab_partie[i].joueur2.csock, &readfds))
			{
				Move coordonne;
				memset(&coordonne, -1, sizeof( coordonne) );	
				
				memset(buffer, 0, BUFFSIZE);
				memset(newTb, 0, BUFFSIZE);
				
				rec = socket_recv( tab_partie[i].joueur2.csock , &coordonne , sizeof(coordonne));

				if(rec==0)
				{

					// juste si on veut informer les autres qu'un joueur est parti
					/*for(j = 0; j < nb_c; j++)
					{
						if( j != i)
						{
						
                            memset(quitte, 0, BUFFSIZE);
						    strcat(quitte, "Serveur : ");
						    strcat(quitte,tab_client[i].pseudo);
				            strcat(quitte," a quitte le jeu :(\n");
							send_client(tab_client[j].csock, quitte);
						}
					}
					*/
					rmv_client(tab_client, i, &nb_c);

					
				}else{
					
					
					UpdatePoint( &coordonne.from, coordonne.from.x, 7 - coordonne.from.y, coordonne.from.color );
					UpdatePoint( &coordonne.to, coordonne.to.x, 7 - coordonne.to.y, coordonne.to.color );

					printf("----------------DEBUGGING indices player2-----------\n\n");
					ShowPoint(&coordonne.from);
					ShowPoint(&coordonne.to);
					ShowPoint(&coordonne.eaten);
					printf("----------------end debug indices player2------------\n\n");
					// translate only if there is a pawn to eat
					if( coordonne.eaten.x > -1 && coordonne.eaten.y > -1 )
					{
						UpdatePoint(&coordonne.eaten, coordonne.eaten.x, 7 - coordonne.eaten.y, coordonne.eaten.color);
					}

					printf("----------------DEBUGGING indices updated player2-----------\n\n");
					ShowPoint(&coordonne.from);
					ShowPoint(&coordonne.to);
					ShowPoint(&coordonne.eaten);
					printf("----------------end debug indices updated player2------------\n\n");
					socket_send( tab_partie[i].joueur1.csock , &coordonne, sizeof(coordonne));

					if( tab_partie[i].nbSpectateur != 0 ){
					
						for( j=0; j< tab_partie[i].nbSpectateur; j++){
						
							socket_send( tab_partie[i].tabSpectateur[j].csock , &coordonne, sizeof(coordonne));						
						}

					}					
				}
			}
			
			
		}
		
		// on regarde si un spectateur a envoyer quelque chose si oui alors on l'enleve
		for(i = 0; i < nb_joueur/2; i++){
			int nbSpectateur = tab_partie[i].nbSpectateur;

			for(j = 0; j < nbSpectateur; j++){


				if( FD_ISSET( tab_partie[i].tabSpectateur[j].csock, &readfds)){
					// alors on enleve ce spectateur

					rec = socket_recv( tab_partie[i].tabSpectateur[j].csock , buffer , BUFFSIZE);
					if( rec == 0){
						rmv_client( tab_partie[i].tabSpectateur, j, &tab_partie[i].nbSpectateur);	
					}

				}	
			}
	    }
			
	}
	
	return 0;
}

