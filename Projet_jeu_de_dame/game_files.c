#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_sockets.h"
#include "game_login.h"
#include "game_files.h"

char *tab_pseudo[MAX_CLIENT]; // tableau qui va contenir 10 pseudo
char *tab_motDepasse[MAX_CLIENT];
int nbJoueurActuel = 0; // c'est le nombre de joueur actuel dans la base de donnee pas dans le jeu

void allocation_tab_pseudo( char *tab_pseudo[], char *tab_motDepasse[]){

	int i;
	for(i = 0; i < MAX_CLIENT; i++){

		tab_pseudo[i]= malloc(sizeof(char)*10);
		memset(tab_pseudo[i], '\0', 10);
		tab_motDepasse[i]= malloc(sizeof(char)*10);
		memset(tab_motDepasse[i], '\0', 10);
	}
	
}

FILE * ouvrirFichier(const char *nom_fichier){

	

    FILE * mon_flux = fopen( nom_fichier, "r+" );
    
    if ( mon_flux == NULL ){
    	
    	fonc_perror( "fopen()");
    }
    
    return mon_flux  ;
}

void ajouterJoueur( char pseudo[], char motpasse[], char* tab_pseudo[], char* tab_motDepasse[], int *nbJoueur){
	
	
	//printf("pseudo : %s ;;; mot de passe : %s\n", pseudo, motpasse);
	
	FILE *flux = ouvrirFichier("gameDataBase.txt");
	fseek(flux, 0, SEEK_END);
	int val_recupere = fprintf(flux,"%s;%s\n", pseudo, motpasse);
	fclose(flux);

	//printf("val_recupere : %d\n", val_recupere);
	*nbJoueur = *nbJoueur + 1;
	
	//printf("nbJoueur : %d\n", *nbJoueur);
	
	strncpy(tab_pseudo[*nbJoueur], pseudo, strlen(pseudo));
	strncpy(tab_motDepasse[*nbJoueur], motpasse, strlen(motpasse));
	
	if(val_recupere < 0){
		fonc_perror("fprintf()");	
	}

}

void recupererJoueurs( FILE *flux, char *tab_pseudo[], char *tab_motDepasse[], int *nbJoueur){

	char *tab = (char*)calloc(BUFFSIZE , sizeof(char));
    int a;
    char pseudo[10]={0};
    char motpasse[10]={0};
    int i = 0;
    do{
    
       	a = fread(tab, 1, sizeof(char), flux);
       	
       	if( strncmp(tab, ";", 1) != 0 ){
       	
       		strcat( pseudo, tab);     
       	    memset(tab, 0, 100);
       	
       	}else{

    		do{

    			memset(tab, 0, 100);    			
    			fread(tab, 1, sizeof(char), flux);
    			strcat( motpasse, tab);       	    	

    				
    		}while( a > 0 && strncmp( tab , "\n", 1 ) != 0);   
    		

    		motpasse[strlen(motpasse)-1]='\0';  
    		
    		strncpy((char*)tab_pseudo[i], pseudo, strlen(pseudo));
    		strncpy((char*)tab_motDepasse[i], motpasse, strlen(motpasse));
    		
    		memset(pseudo, 0, 10);
    		memset(motpasse, 0, 10);
    		i++;
       	}
	


    }while(a > 0);

	free(tab);
	
	*nbJoueur =i;


}


