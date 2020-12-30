#ifndef _GAME_LOGIN_H
#define _GAME_LOGIN_H

#include <stdio.h>

#include "game_engine.h"

struct Client definirModeConnexion( int *nb_c, char *buffer, struct Client ret_client, FILE *flux);
struct Client modeConnexion( struct Client ret_client, char *buffer, int*nb_c, FILE *flux );
int identificationServeur( char *buffer, struct Client ret_client, char *pseudo, char *motDepasse);
int verification_pseudo_mot(char *tab_pseudo[], char *tab_mot_passe[], char pseudo[], char mot_passe[]);
// je peux retourner ret_client et ajouter le mode ici
struct Client jouerOUregarderPartie( struct Client ret_client, char *buffer);
struct Client creationCompte( struct Client ret_client, char *buffer, FILE *flux);
void envoyerCouleur( struct Client ret_client, int nb_c );
char definirCouleur( int  nb_c );

#endif

