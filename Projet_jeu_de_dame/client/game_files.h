#ifndef _GAME_FILES_H
#define _GAME_FILES_H

#include "game_engine.h"

extern char *tab_pseudo[MAX_CLIENT]; // tableau qui va contenir 10 pseudos
extern char *tab_motDepasse[MAX_CLIENT];
extern int nbJoueurActuel;// c'est le nombre de joueur actuel dans la base de donnee pas dans le jeu
void allocation_tab_pseudo( char *tab_pseudo[], char *tab_motDepasse[]);
FILE * ouvrirFichier(const char *nom_fichier);
void ajouterJoueur( char pseudo[], char motpasse[], char* tab_pseudo[], char* tab_motDepasse[], int *nbJoueur);
void recupererJoueurs( FILE *flux, char *tab_pseudo[], char *tab_motDepasse[], int *nbJoueur);

#endif

