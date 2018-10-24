#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_video.h>
#include "../Libraries/GCVec2dLib.h"
#include "../Libraries/GCGraLib.h"
#include "../Libraries/graph_code.h"
#include "../ai/y_reduction.h"
#include "../ai/min_max.h"

time_t start,end,t0,t1,t2,t3;

int players;// 0 se giocatore-giocatore, 1 se giocatore-computer

int vincitore;// 0 per il rosso, 1 per il verde, 2 per il computer

SDL_Surface *screen;
SDL_Surface *img;
SDL_Surface *spiral;
SDL_Surface *fantasy;
SDL_Rect rect;
Graph graph;
SDL_Event event;
TTF_Font *font;
TTF_Font *font2;

//variabili di gioco

int ai;//1 per Y REDUCTION, 2 per MIN MAX

int dimensione;//dimensione della scacchiera
double spazio =1;//variabile di shift usata per creare gli esagoni
int finestra =600;//dimensione della finestra
int ccc=0;//variabile di controllo temporanea del giocatore iniziale
int position;//posizione attuale della pedina
int giocatoreuno=0;// con 0 inizia il rosso, con 1 il verde
int vincita;//controlla se qualcuno ha vinto
int rad;//raggio delle pedine
double dist_min;//distanza dal centro dell'esagono piu'vicino
int ix,iy;//mouse
int f;//contatore delle mosse
IntPoint2 mouse;//coordinate del mouse
Point2 centrototale;//centro della scacchiera
int controlloswap;//controlla lo swap
int giocatoreiniziale1;//1 se inizia l'umano, 0 se inizia il pc
int recursion_level;//profondita pc
int recursion_level_con=2;//profondita consigli
int controllogiocatoreiniziale;//0 se non ha ancora deciso, 1 se si
int swap1;//controlla se lo swap e' avvenuto o no
int conta; // conta l'indicizzazione della reduction

//orologio
struct tm *tPre;
time_t timePresent;
char orologio[9];
int secondi=-1;

//funzioni
int tastiera();
int exit_reset();
void schermata_iniziale();
void scelta();
void gioco();
void vinto();
void mossa_swap();
void stampa_scacchiera();
void scritte();
void azzera();
void aggiorna_pagina();
void scelta_giocatori();

//array
int ***x;
int ***y;
IntPoint2 *probabilita;
int **current;//1 per il rosso, 2 per il verde
Point2 ***punti_finali;
IntPoint2 **centri;
Point2 **point;
int *NodiEliminati;
IntPoint2 *history;
int *supporto;
double *prob;
int **path;
int **blind;
int *sostituzioni;


void indicizzazione (){
	int i,j,k;
	conta=0;
	for (k=0; k<2*dimensione-1; k++) {
		for(i=0;i<2*dimensione-2-k;i++){
			for (j=0; j<=i; j++){
				if (j<dimensione &&j>i-dimensione ) {
					sostituzioni[conta]=supporto[i]-j;
					conta++;
					sostituzioni[conta]=supporto[i+1]-j;
					conta++;
					sostituzioni[conta]=supporto[i+1]-1-j;
					conta++;
				}
			}
			
		}
	}
}

void forza_orologio(){
	timePresent= time(NULL);
	tPre = localtime(&timePresent);
	secondi=tPre->tm_sec%10+48;
	orologio[0]=(char)(tPre->tm_hour/10+48);
	orologio[1]=(char)(tPre->tm_hour%10+48);
	
	orologio[3]=(char)(tPre->tm_min/10+48);
	orologio[4]=(char)(tPre->tm_min%10+48);
	
	orologio[6]=(char)(tPre->tm_sec/10+48);
	orologio[7]=(char)(tPre->tm_sec%10+48);
	
	GC_FillCircle(screen, 175, -37, 67, colori.black);
	GC_DrawCircle(screen, 175, -37, 67, colori.red);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,orologio,140,0,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void orolo(){
	timePresent= time(NULL);
	tPre = localtime(&timePresent);
	if (secondi!=tPre->tm_sec%10+48){
		secondi=tPre->tm_sec%10+48;
		orologio[0]=(char)(tPre->tm_hour/10+48);
		orologio[1]=(char)(tPre->tm_hour%10+48);
		orologio[3]=(char)(tPre->tm_min/10+48);
		orologio[4]=(char)(tPre->tm_min%10+48);
		orologio[6]=(char)(tPre->tm_sec/10+48);
		orologio[7]=(char)(tPre->tm_sec%10+48);
		GC_FillCircle(screen, 175, -37, 67, colori.black);
		GC_DrawCircle(screen, 175, -37, 67, colori.red);
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,orologio,140,0,blended);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
}

void inizializza_colori(){
	colori.red=SDL_MapRGB(screen->format,255,0,0);
	colori.green=SDL_MapRGB(screen->format,50,205,50);
	colori.rectangle=SDL_MapRGB(screen->format,200,200,200);
	colori.black=SDL_MapRGB(screen->format,0,0,0);
	colori.white=SDL_MapRGB(screen->format,255,255,255);
	colori.rosso=SDL_MapRGB(screen->format,0,0,255);
}

void esci(){
	SDL_FreeSurface(screen);
	SDL_FreeSurface(img);
	SDL_FreeSurface(spiral);
	SDL_FreeSurface(fantasy);
	TTF_CloseFont(font);
	TTF_CloseFont(font2);
	TTF_Quit();
	SDL_Quit();
	exit(0);
}

void blank(){
	SDL_FillRect(screen, &rect, colori.black);
}

//funzioni creazione grafo/scacchiera

void metti_pedina_rossa(int i, int j){
	position= i*dimensione+j;
	GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.rosso);
	GC_DrawCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.white);
	Graph_isolNode(position+2-NodiEliminati[position],&graph);
	current[i][j]=1;
	history[f].x= i;
	history[f].y= j;
	vincita=Graph_DFS(1,dimensione*dimensione+2-NodiEliminati[dimensione*dimensione-1], &graph);
	if(vincita==0)
		GC_FillCircle(screen, finestra*0.10+180,finestra*0.9+10, 10, colori.green);
	f++;
}

void metti_pedina_verde(int i, int j){
	int k;
	position= i*dimensione+j;
	GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.green);
	GC_DrawCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.white);
	Graph_contract(position+2-NodiEliminati[position],&graph);
	for(k=position;k<dimensione*dimensione;k++)
		NodiEliminati[k]++;
	current[i][j]=2;
	history[f].x= i;
	history[f].y= j;
	if (Graph_minPath(1,dimensione*dimensione+2-NodiEliminati[dimensione*dimensione-1], &graph)==1 )
		vincita=1;
	if(vincita==0)
		GC_FillCircle(screen, finestra*0.10+180,finestra*0.9+10, 10, colori.rosso);
	f++;
}

void scritte_gioco(){
	if(vincita==1){
		if(vincitore==0){
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL GIOCATORE ROSSO!!!",finestra*0.10,finestra*0.9,blended);
		}
		if(vincitore==1){
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL GIOCATORE VERDE!!!",finestra*0.10,finestra*0.9,blended);
		}
		if(vincitore==2){
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL COMPUTER!!!",finestra*0.10,finestra*0.9,blended);
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		vinto();
	}
	else{
		
		if (controllogiocatoreiniziale==0 || (f==1 && controlloswap==0)){
			if (controllogiocatoreiniziale==0 &&  players!=4){
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Quale giocatore inizia?",finestra*0.10,finestra*0.9,blended);
				GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"Verde",finestra*0.50,finestra*0.9,blended);
				GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Rosso",finestra*0.60,finestra*0.9,blended);
				GC_DrawRect(screen,finestra*0.50-1,finestra*0.9-1,50,22,colori.rectangle);
				GC_DrawRect(screen,finestra*0.60-1,finestra*0.9-1,60,22,colori.rectangle);
			}
			else if(f==1 && controlloswap==0 && players!=4){
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Vuoi rubargli la mossa?",finestra*0.10,finestra*0.9,blended);
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0," Y",finestra*0.50,finestra*0.9,blended);
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0," N",finestra*0.60,finestra*0.9,blended);
				GC_DrawRect(screen,finestra*0.50-1,finestra*0.9-1,25,22,colori.rectangle);
				GC_DrawRect(screen,finestra*0.60-1,finestra*0.9-1,25,22,colori.rectangle);
			}
		}
		else{
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"turno del giocatore:",finestra*0.10,finestra*0.9,blended);
			if ( f%2 == giocatoreuno)
				GC_FillCircle(screen, finestra*0.10+180,finestra*0.9+10, 10, colori.rosso);
			else
				GC_FillCircle(screen, finestra*0.10+180,finestra*0.9+10, 10, colori.green);
		}
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

int load_file(){
	int i,j;
	printf("sto caricando il file\n");
	FILE *stream=fopen("TXT/hex_scheme.txt", "r");
	if (stream == NULL) {
		fprintf(stderr, "Can't open input file!\n");
		esci();
	}
	int dim;
	fscanf(stream,"%d ",&dim);
	fscanf(stream,"%d ",&giocatoreuno);
	fscanf(stream,"%d ",&controlloswap);
	fscanf(stream,"%d ",&players);
	fscanf(stream,"%d ",&ai);
	fscanf(stream,"%d ",&recursion_level);
	fscanf(stream,"%d ",&vincita);
	fscanf(stream,"%d ",&vincitore);
	dimensione=dim;
	blank();
	azzera();
	aggiorna_pagina();
	int numerodimosse=0;
	dim=0;
	
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			
			fscanf(stream,"%d ",&dim);
			if(dim==1){
				metti_pedina_rossa(i,j);
				numerodimosse++;
			}
			
			if(dim==2){
				metti_pedina_verde(i,j);
				numerodimosse++;
			}
		}
	}
	
	fclose(stream);
	
	f=numerodimosse;
	scritte_gioco();
	
	printf("file caricato \n");
	gioco();
	return(1);
}

void save_file(){
	int i,j;
	printf("file salvato\n");
	FILE *stream=fopen("TXT/hex_scheme.txt", "w");
	if (stream == NULL) {
		fprintf(stderr, "Can't open output file!\n");
		esci();
	}
	fprintf(stream,"%d ",dimensione);
	fprintf(stream,"%d ",giocatoreuno);
	fprintf(stream,"%d ",controlloswap);
	fprintf(stream,"%d ",players);
	
	fprintf(stream,"%d ",ai);
	fprintf(stream,"%d ",recursion_level);
	fprintf(stream,"%d ",vincita);
	fprintf(stream,"%d ",vincitore);
	fprintf(stream,"\n");
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			fprintf(stream,"%d ",current[i][j]);
		}
		fprintf(stream,"\n");
	}
	fclose(stream);
}

void crea_scacchiera(){
	int i,j,k;
	VIEWPORT view={0, finestra, 0, finestra}; // la seconda finestra e' libera
	WINDOW win={-1.0, 1.0, -1.0, 1.0};
	//creazione dei centri per gli esagoni
	point[0][0].x=spazio;
	point[0][0].y=-spazio;
	
	for (i=1;i<dimensione;i++){
			point[i][0].x= point[i-1][0].x+spazio/2;
			point[i][0].y= point[i-1][0].y-spazio/2*SQRT3;
	}
	
	for (i=0;i<dimensione;i++)
		for (k=1;k<dimensione;k++){
			point[i][k].x= point[i][k-1].x+spazio;
			point[i][k].y= point[i][k-1].y;
		}
    
	//costruzione esagoni
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			punti_finali[i][j][0].y=point[i][j].y+(spazio/SQRT3);
			punti_finali[i][j][0].x=point[i][j].x;
			for(k=1;k<6;k++){
				punti_finali[i][j][k]=ruota_punto(punti_finali[i][j][k-1], point[i][j], PI/3);
			}
		}
	}
	
	// creazione del centro della scacchiera
	centrototale.x = (point[dimensione-1][dimensione-1].x + point[0][0].x)/2;
	centrototale.y = (point[dimensione-1][dimensione-1].y + point[0][0].y)/2;
	
	// rotazione dei punti per renderlo a diamante
	for (i=0;i<dimensione;i++)
		for (k=0;k<dimensione;k++){
			point[i][k]= ruota_punto(point[i][k], centrototale, PI/6);
		}
		
	// rotazione degli esagoni
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			for(k=0;k<6;k++){
				punti_finali[i][j][k]=ruota_punto(punti_finali[i][j][k], centrototale, PI/6);
			}
		}
	}
	
	// traslazione per rendere tutte le x positive
	
	double shift = punti_finali[0][0][1].x;
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			for(k=0;k<6;k++){
				punti_finali[i][j][k].x-=shift;
			}
			point[i][j].x-=shift;
		}
	}
	centrototale.x-= shift;
	
	//redimensiona tutto ( tra -1 e 1 )
	
	double fattore = 2/punti_finali[dimensione-1][dimensione-1][4].x;
    
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			point[i][j].y=point[i][j].y*fattore+0.7;
            point[i][j].x=point[i][j].x*fattore-1;
		}
	}
    
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			for(k=0;k<6;k++){
               	 		punti_finali[i][j][k].y=punti_finali[i][j][k].y*fattore+0.7;
               			punti_finali[i][j][k].x=punti_finali[i][j][k].x*fattore-1;
			}
		}
	}
    
	// ridimensiona ulteriormente ( 0 - 400 )
	
    
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			wind_view(point[i][j].x, point[i][j].y,&centri[i][j].x,&centri[i][j].y,view,win);
		}
	}
	
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			for(k=0;k<6;k++)
				wind_view(punti_finali[i][j][k].x,punti_finali[i][j][k].y,&x[i][j][k],&y[i][j][k],view,win);
}

void stampa_scacchiera(SDL_Surface *sur){
	int i,j,k;
	// stampa la griglia
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			for(k=0;k<5;k++){
				GC_DrawLine(sur, x[i][j][k], y[i][j][k], x[i][j][k+1], y[i][j][k+1], colori.black);
			}
			GC_DrawLine(sur, x[i][j][5], y[i][j][5], x[i][j][0], y[i][j][0], colori.black);
		}
	}
	
	// stampa le linee
	GC_DrawLine(sur, x[0][0][0], y[0][0][0], x[0][dimensione-1][0], y[0][dimensione-1][0], colori.green);
	GC_DrawLine(sur, x[dimensione-1][0][3], y[dimensione-1][0][3], x[dimensione-1][dimensione-1][3], y[dimensione-1][dimensione-1][3], colori.green);
    
	GC_DrawLine(sur, x[0][0][2], y[0][0][2], x[dimensione-1][0][2], y[dimensione-1][0][2], colori.red);
	GC_DrawLine(sur, x[0][dimensione-1][5], y[0][dimensione-1][5], x[dimensione-1][dimensione-1][5], y[dimensione-1][dimensione-1][5], colori.red);
	
	GC_DrawLine(sur, x[0][0][0]-1, y[0][0][0], x[0][dimensione-1][0], y[0][dimensione-1][0]-1, colori.green);
	GC_DrawLine(sur, x[dimensione-1][0][3]+1, y[dimensione-1][0][3], x[dimensione-1][dimensione-1][3], y[dimensione-1][dimensione-1][3]+1, colori.green);
    
	GC_DrawLine(sur, x[0][0][2]-1, y[0][0][2], x[dimensione-1][0][2], y[dimensione-1][0][2]+1, colori.red);
	GC_DrawLine(sur, x[0][dimensione-1][5]+1, y[0][dimensione-1][5], x[dimensione-1][dimensione-1][5], y[dimensione-1][dimensione-1][5]-1, colori.red);
}

void scritte(SDL_Surface *sur){
	GC_DrawText(sur,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(sur,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
    	GC_DrawText(sur,font,255,255,255,0,0,0,0,0,"Restart",42,0,blended);
	GC_DrawText(sur,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+4,0,blended);
	GC_DrawText(sur,font,255,255,255,0,0,0,0,0,"Options",finestra-150+4,0,blended);
	GC_DrawRect(sur,0,0,35,21,colori.rectangle);
	GC_DrawRect(sur,35,0,80,21,colori.rectangle);
	GC_DrawRect(sur,finestra-60,0,60,21,colori.rectangle);
	GC_DrawRect(sur,finestra-150,0,90,21,colori.rectangle);
}

void create_edges(Graph *f){
	int i,j;
	// collega i primi al punto iniziale
	for(i=2;i<=dimensione+1;i++)
		Graph_addEdge(1, i, f);
	
	//crea i collegamenti per la prima riga
	for(i=2;i<=dimensione;i++)
		Graph_addEdge(i, i+1, f);
	
	//crea i collegamenti per l'ultima colonna
	for(i=dimensione+1; i<dimensione*dimensione; i+=dimensione)
		Graph_addEdge(i, i+dimensione, f);	
	
	//crea tutti i collegamenti intermedi tra i punti
	for(j=1;j<dimensione;j++)
		for(i=j*dimensione+2;i<(j+1)*dimensione+1;i++){
			Graph_addEdge(i, i-dimensione, f);
			Graph_addEdge(i, i-dimensione+1, f);
			Graph_addEdge(i, i+1, f);
			}
			
	//crea i collegamenti con il punto finale
	for(i=dimensione*(dimensione-1)+2;i<=dimensione*dimensione+1;i++)
		Graph_addEdge(i,dimensione*dimensione+2, f);
}

void azzera(){
	free(x);
	free(y);
	free(current);
	free(punti_finali);
	free(centri);
	free(point);
	free(NodiEliminati);
	free(history);
	free(prob);
	free(supporto);
	free(path);
	free(blind);
	free(probabilita);
	free(sostituzioni);
	
	int i,j;
	
	probabilita= malloc((dimensione*dimensione)*sizeof(IntPoint2));
	
	prob=malloc((2*dimensione-1)*dimensione*sizeof(double));
	
	supporto=malloc((2*dimensione-1)*sizeof(int));
	
	//path e blind
	path = malloc(dimensione*sizeof(int **));
	
	for (i = 0; i < dimensione; ++i)
		path[i] = malloc(dimensione*sizeof(int *));
	blind = malloc(dimensione*sizeof(int **));
	
	for (i = 0; i < dimensione; ++i)
		blind[i] = malloc(dimensione*sizeof(int *));
	
	
	//x
	x = malloc(dimensione*sizeof(int **));
	
	for (i = 0; i < dimensione; ++i)
		x[i] = malloc(dimensione*sizeof(int *));
	
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			x[i][j] = malloc(6*sizeof(int));
	//y
	y = malloc(dimensione*sizeof(int **));
	
	for (i = 0; i < dimensione; ++i)
		y[i] = malloc(dimensione*sizeof(int *));
	
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			y[i][j] = malloc(6*sizeof(int));
		
	//punti_finali
	punti_finali = malloc(dimensione*sizeof(Point2 **));
	
	for (i = 0; i < dimensione; ++i)
		punti_finali[i] = malloc(dimensione*sizeof(Point2 *));
	
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			punti_finali[i][j] = malloc(6*sizeof(Point2));
		
	//point
	point = malloc(dimensione*sizeof(Point2 *));
	for (i = 0; i < dimensione; ++i)
		point[i] = malloc(dimensione*sizeof(Point2));
	
	//centri
		
	centri = malloc(dimensione*sizeof(IntPoint2*));
	for (i = 0; i < dimensione; ++i)
		centri[i] = malloc(dimensione*sizeof(IntPoint2));
	
	//current
	
	current = malloc(dimensione*sizeof(int *));
	for (i = 0; i < dimensione; ++i)
		current[i] = malloc(dimensione*sizeof(int));
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			current[i][j]=0;
		
	NodiEliminati= malloc(dimensione*dimensione*sizeof(int));
	
	history=malloc(dimensione*dimensione*sizeof(Point2));
	
	// Y_reduction
	supporto[0]=0;
	int temp=0;
	for(i=2;i<2*dimensione;i++){
		temp +=i;
		supporto[i-1]=temp;
	}
	sostituzioni=malloc(4*supporto[2*dimensione-2]*supporto[2*dimensione-2]*sizeof(int));
	indicizzazione();
	crea_scacchiera();
	
	free_Graph(&graph);
	create_Graph(dimensione*dimensione+2,&graph);
	create_edges(&graph);
	
	rad=(y[0][0][3]-y[0][0][0])/3;
	for(i=0; i<dimensione*dimensione;i++)
		NodiEliminati[i]=0;
	
	swap1=0;
	vincita=0;
	f=0;
	ccc=0;
	controlloswap=0;
	for(i=0;i<dimensione;i++)
		for(j=0;j<dimensione;j++)
			current[i][j]=0;
	SDL_FillRect(screen, &rect, colori.black);
	SDL_FillRect(img, &rect, colori.black);
	
	scala_immagine(img, finestra, "Images/images.jpg");
	stampa_scacchiera(img);
	scritte(img);
	blank();
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

// gioco

IntPoint2 min_max_green(){
	return min_max(current, recursion_level, 2,prob,dimensione, supporto,path,blind,f,controlloswap,conta,sostituzioni);
}

IntPoint2 min_max_red(){
	return min_max(current, recursion_level, 1,prob,dimensione, supporto,path,blind,f,controlloswap,conta, sostituzioni);
}

void aggiorna_pagina(){
	int i,j;
	SDL_BlitSurface(img,NULL,screen,NULL);
	if(players==0){
	  	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HUM",5,50,blended);
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HUM",finestra-60,50,blended);
	}
	if(players==1){
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HUM",5,50,blended);
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"PC",finestra-60,50,blended);
		if(ai==2){
			if(recursion_level==1)
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max(1)",finestra-100,80,blended);
			if(recursion_level==2)
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max(2)",finestra-100,80,blended);
			if(recursion_level==3)
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max(3)",finestra-100,80,blended);
			if(recursion_level==4)
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max(4)",finestra-100,80,blended);
			if(recursion_level==5)
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max(5)",finestra-100,80,blended);
		}
		else
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"y-red",finestra-80,80,blended);
		
	}
	if(players!=0){
		if(ai==2){
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"pruning on",finestra-100,120,blended);
		}
	}
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			if(current[i][j]==1){
				GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.rosso);
				GC_DrawCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.white);
			}
			
			if(current[i][j]==2){
				GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.green);
				GC_DrawCircle(screen, centri[i][j].x, centri[i][j].y, rad, colori.white);
			}
		}
	}
	orolo();
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void aggiorna_history(){
	FILE *stream;
	stream=fopen("TXT/hex_history.txt", "w");
	if (stream == NULL) {
		fprintf(stderr, "Can't open output file!\n");
		esci();
	}
	int i;
	for(i=0;i<f;i++)
		fprintf(stream,"%d   %d %d\n",i,history[i].x,history[i].y);
	fclose(stream);
}

void escape(){
	if(event.key.keysym.sym==SDLK_ESCAPE){
		SDL_FreeSurface(screen);
		SDL_FreeSurface(img);
		TTF_CloseFont(font);
		TTF_CloseFont(font2);
		TTF_Quit();
		SDL_Quit();
		exit(0);
	}
}
  
void metti_pedina(int ax, int ay){
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	//rosso
	if ( f%2 == giocatoreuno){
		metti_pedina_rossa(ax, ay);
		if(vincita==1){
			aggiorna_pagina();
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL GIOCATORE ROSSO!!!",finestra*0.10,finestra*0.9,blended);
			vincitore=0;
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			vinto();
			return;
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
	//verde
	else {
		metti_pedina_verde(ax,ay);
		if(vincita==1){
			aggiorna_pagina();
			GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL GIOCATORE VERDE!!!",finestra*0.10,finestra*0.9,blended);
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			vincitore=1;
			vinto();
			return;
		}
		SDL_UpdateRect(screen, 0, 0, 0, 0);
	}
}

void giocatore_iniziale(){
	//con zero inizia il rosso, con uno inizia il verde
	forza_orologio();
	controllogiocatoreiniziale=0;
	aggiorna_pagina();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Quale giocatore inizia?",finestra*0.10,finestra*0.9,blended);
	GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"Verde",finestra*0.50,finestra*0.9,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Rosso",finestra*0.60,finestra*0.9,blended);
	GC_DrawRect(screen,finestra*0.50-1,finestra*0.9-1,50,22,colori.rectangle);
	GC_DrawRect(screen,finestra*0.60-1,finestra*0.9-1,60,22,colori.rectangle);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		orolo();
		if (SDL_PollEvent(&event)){
			if( event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				if (exit_reset()==1)
					continue;
				if (mouse.x<finestra*0.50+49 && mouse.x>finestra*0.50-1 && mouse.y<finestra*0.9+21 && mouse.y>finestra*0.9-1){
					giocatoreuno=1;
					controllogiocatoreiniziale=1;
					break;
				}
				if (mouse.x<finestra*0.60+59 && mouse.x>finestra*0.60-1 && mouse.y<finestra*0.9+21 && mouse.y>finestra*0.9-1){
					giocatoreuno=0;
					controllogiocatoreiniziale=1;
					break;
				}
			}
			else if (event.type == SDL_KEYDOWN){
				if(event.key.keysym.sym==SDLK_v){
					giocatoreuno=1;
					controllogiocatoreiniziale=1;
					break;
				}
				if(event.key.keysym.sym==SDLK_r){
					giocatoreuno=0;
					controllogiocatoreiniziale=1;
					break;
				}
				tastiera();
			}
		}
	}
	aggiorna_pagina();
	ccc=1;
	scritte_gioco();
	forza_orologio();
}

void mossa_swap(){
	forza_orologio();
	int i;
	controlloswap=1;
	for(i=0;i<dimensione*dimensione;i++)
		NodiEliminati[i]=0;
	if ( players!=0){
		iy=history[0].y;
		ix=history[0].x;
	}
	position= iy*dimensione+ix;
	
	if(giocatoreuno==1){// il primo a giocare era il verde
		GC_FillCircle(screen, centri[history[0].y][history[0].x].x  ,  centri[history[0].y][history[0].x].y, rad, colori.rosso);
		GC_DrawCircle(screen, centri[history[0].y][history[0].x].x  ,  centri[history[0].y][history[0].x].y, rad, colori.white);
		free_Graph(&graph);
		create_Graph(dimensione*dimensione+2,&graph);
		create_edges(&graph);
		Graph_isolNode(dimensione*history[0].y+history[0].x+2,&graph);
		giocatoreuno=0;
		if(players==1){
			if(giocatoreiniziale1==0)
				giocatoreiniziale1=1;
			else
				giocatoreiniziale1=0;
		}
		f=1;
	}
	else{// il primo a giocare era il rosso
		GC_FillCircle(screen, centri[history[0].y][history[0].x].x  ,  centri[history[0].y][history[0].x].y, rad, colori.green);
		GC_DrawCircle(screen, centri[history[0].y][history[0].x].x  ,  centri[history[0].y][history[0].x].y, rad, colori.white);
		create_edges(&graph);
		Graph_contract(dimensione*history[0].y+history[0].x+2,&graph);
		f=1;
		giocatoreuno=1;
		if(players==1){
			if(giocatoreiniziale1==0)
				giocatoreiniziale1=1;
			else
				giocatoreiniziale1=0;
		}
		for(i=position;i<dimensione*dimensione;i++)
			NodiEliminati[i]++;
	}
	current[history[0].x][history[0].y]=0;
	current[history[0].y][history[0].x]=giocatoreuno+1;
	
	FILE *stream;
	stream=fopen("TXT/hex_history.txt", "a");
	if (stream == NULL) {
		fprintf(stderr, "Can't open output file!\n");
		esci();
	}
	fprintf(stream,"il giocatore ha swappato\n");
	fclose(stream);
	aggiorna_pagina();
	scritte_gioco();
	forza_orologio();
	swap1=1;
}

void swap(){
	if(controlloswap==0){
		aggiorna_pagina();
		forza_orologio();
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Vuoi rubargli la mossa?",finestra*0.10,finestra*0.9,blended);
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0," Y",finestra*0.50,finestra*0.9,blended);
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0," N",finestra*0.60,finestra*0.9,blended);
		GC_DrawRect(screen,finestra*0.50-1,finestra*0.9-1,25,22,colori.rectangle);
		GC_DrawRect(screen,finestra*0.60-1,finestra*0.9-1,25,22,colori.rectangle);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		while(1){
			orolo();
			if (SDL_PollEvent(&event)){
				if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
					mouse.x=event.button.x;
					mouse.y=event.button.y;
					
					if(exit_reset()==1)
						continue;
					if (mouse.x<finestra*0.50+24 && mouse.x>finestra*0.50-1 && mouse.y<finestra*0.9+21 && mouse.y>finestra*0.9-1){
						mossa_swap();
						orolo();
						break;
					}
					if (mouse.x<finestra*0.60+24 && mouse.x>finestra*0.60-1 && mouse.y<finestra*0.9+21 && mouse.y>finestra*0.9-1){
						controlloswap=1;
						aggiorna_pagina();
						scritte_gioco();
						forza_orologio();
						break;
					}
				}
				else if(event.type == SDL_KEYDOWN){
					if(event.key.keysym.sym==SDLK_y){
						mossa_swap();
						orolo();
						break;
					}
					if(event.key.keysym.sym==SDLK_n){
						controlloswap=1;
						aggiorna_pagina();
						scritte_gioco();
						forza_orologio();
						break;
					}
					tastiera();
				}
			}
		}
	}
}

void scritte_menu_scelte(){
	blank();
	SDL_BlitSurface(fantasy,NULL,screen,NULL);
}

void menu_scelte(){
	scritte_menu_scelte();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"salva",finestra/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"carica",finestra/3+5,finestra/8*2+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"aggiorna history",finestra/3+5,finestra/8*3+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"torna al gioco",finestra/3+5,finestra/8*4+7,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8 && event.motion.y<finestra/8+40){
					scritte_menu_scelte();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"salva",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"carica",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"aggiorna history",finestra/3+5,finestra/8*3+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"torna al gioco",finestra/3+5,finestra/8*4+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+40){
					scritte_menu_scelte();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"salva",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"carica",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"aggiorna history",finestra/3+5,finestra/8*3+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"torna al gioco",finestra/3+5,finestra/8*4+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+40){
					scritte_menu_scelte();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"salva",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"carica",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"aggiorna history",finestra/3+5,finestra/8*3+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"torna al gioco",finestra/3+5,finestra/8*4+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*4 && event.motion.y<finestra/8*4+40){
					scritte_menu_scelte();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"salva",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"carica",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"aggiorna history",finestra/3+5,finestra/8*3+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"torna al gioco",finestra/3+5,finestra/8*4+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8 && mouse.y<finestra/8+40){
					save_file();
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+40){
					load_file();
					gioco();
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+40){
					aggiorna_history();
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*4 && mouse.y<finestra/8*4+40){
					return;
				}
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			else if(event.type == SDL_KEYDOWN){
				escape();
				if(event.key.keysym.sym==SDLK_s){
					save_file();
					return;
				}
				if(event.key.keysym.sym==SDLK_l){
					load_file();
					return;
				}
				if(event.key.keysym.sym==SDLK_h){
					aggiorna_history();
					return;
				}
				if(event.key.keysym.sym==SDLK_h){
					return;
				}
			}
		}
	}
}

int exit_reset(){
	int i,j;
	
	//controlla l'Exit
	if (mouse.x<35 && mouse.y<21){
			esci();
	}
	//controlla il Reset
	if (mouse.x<115 && mouse.x>35 && mouse.y<21){
		free_Graph(&graph);
		create_Graph(dimensione*dimensione+2,&graph);
		create_edges(&graph);
		//toglie le pedine
		for(i=0; i<dimensione*dimensione;i++)
			NodiEliminati[i]=0;
		
		for(i=0;i<dimensione;i++)
			for(j=0;j<dimensione;j++)
				current[i][j]=0;
		vincita=0;
		f=0;
		ccc=0;
		controlloswap=0;
		swap1=0;
		SDL_UpdateRect(screen, 0, 0, 0, 0);
		gioco();
		forza_orologio();
		return(1);
		
	}
	//menu
	if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
		save_file();
		scelta();
		
		return(1);
	}
	//scelte
	if(vincita==0){
		if (mouse.x<finestra-60 && mouse.x>finestra-150 && mouse.y<21){
			menu_scelte();
			aggiorna_pagina();
			scritte_gioco();
			return(2);
		}
	}
	return(2);
}

int tastiera(){
	if (event.key.keysym.sym==SDLK_r){
		save_file();
		scelta();
		return(0);
	}
	//save
	if (event.key.keysym.sym==SDLK_s){
		save_file();
		return (0);
	}
	if (event.key.keysym.sym==SDLK_h){
		aggiorna_history();
		printf("history aggiornata\n");
		return(0);
	}
	//load
	if (event.key.keysym.sym==SDLK_l){
		load_file();
		return(1);
	}
	escape();
	
	return(2);
}

void vinto(){
	
	// stampa il percorso della vittoria
	if(vincitore==0)
		win_rosso(path,blind,current,dimensione);
	if(vincitore==1)
		win_verde(path,blind,current,dimensione);
	if(vincitore==2){
		if(giocatoreiniziale1==0 ){
			if(giocatoreuno==0){
				win_rosso(path,blind,current,dimensione);
			}
			else
				win_verde(path,blind,current,dimensione);
		}
		else{
			if(giocatoreuno==1){
				win_rosso(path,blind,current,dimensione);
			}
			else
				win_verde(path,blind,current,dimensione);
		}
	}
	//volendo si potrebbe provare a eliminare da current le pedine una per una , ricorsivamente, per avere il path minimo di vittoria.
	int i,j;
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			if(path[i][j]==1){
				GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, rad/2, colori.white);
				GC_FillCircle(screen, centri[i][j].x, centri[i][j].y, 2, colori.black);
			}
		}
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	// exit_reset
	while(1){
		orolo();
		if (SDL_PollEvent(&event) ){
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1 ){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				if (exit_reset()==1)
					return;
			}
			if(event.type == SDL_KEYDOWN){
				escape();
			}
		}
	}
}

void scritte_select_level(int a){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",150+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",200+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"3",250+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"4",300+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"5",350+5,finestra/8*2-23,blended);
	
	GC_DrawRect(screen,150,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,200,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,250,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,300,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,350,finestra/8*2,30,30,colori.rectangle);
	if(a==1)
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Seleziona il livello di difficolta'",finestra/4+5,finestra/8+7,blended);
	if(a==2)
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Seleziona il livello di difficolta per il verde",finestra/5+5,finestra/8+7,blended);
	if(a==3)
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Seleziona il livello di difficolta per il rosso",finestra/5+5,finestra/8+7,blended);
}

void select_level(int *level, int a){
	scritte_select_level(a);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if(event.motion.x>150 && event.motion.x<180 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_select_level(a);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
				if(event.motion.x>200 && event.motion.x<230 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_select_level(a);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
				if(event.motion.x>250 && event.motion.x<280 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_select_level(a);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
				if(event.motion.x>300 && event.motion.x<330 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_select_level(a);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
				if(event.motion.x>350 && event.motion.x<380 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_select_level(a);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"1",150+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"2",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"3",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"4",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"5",350+5,finestra/8*2+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1 ){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				
				if(mouse.x>150 && mouse.x<180 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					*level=1;
					return;
				}
				
				if(mouse.x>200 && mouse.x<230 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					*level=2;
					return;
				}
				
				if(mouse.x>250 && mouse.x<280 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					*level=3;
					return;
				}
				
				if(mouse.x>300 && mouse.x<330 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					*level=4;
					return;
				}
				
				if(mouse.x>350 && mouse.x<380 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					*level=5;
					return;
				}
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			
			else if(event.type == SDL_KEYDOWN){
				escape();
				
				if(event.key.keysym.sym==SDLK_1){
					*level=1;
					return;
				}
				
				if(event.key.keysym.sym==SDLK_2){
					*level=2;
					return;
				}
				
				if(event.key.keysym.sym==SDLK_3){
					*level=3;
					return;
				}
				
				if(event.key.keysym.sym==SDLK_4){
					*level=4;
					return;
				}
				
				if(event.key.keysym.sym==SDLK_5){
					*level=5;
					return;
				}
			}
		}
	}
	
	
	
}

void rec_level_pc_1(){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	select_level(&recursion_level,1);
}

int hum(){
	ix=0;
	iy=0;
	//trova il centro piu vicino
	int i,j;
	dist_min= (centri[0][1].x-centri[0][0].x);
	for(i=0;i<dimensione;i++){
		for(j=0;j<dimensione;j++){
			if (dist_min > distanzapunti(&mouse, &centri[i][j])){
				dist_min = distanzapunti(&mouse, &centri[i][j]);
				ix=i;
				iy=j;
			}
		}
	}
	
	if (dist_min < centri[0][1].x-centri[0][0].x && current[ix][iy]==0){
		metti_pedina(ix,iy);
		return(0);
	}
	return(1);
}


void pc_hum(){
	orolo();
	if (f%2==giocatoreiniziale1){
		if((f%2) != giocatoreuno){//verde
			time(&start);
			IntPoint2 temporaneo;
			if(ai==1)
				temporaneo= mossa_migliore(dimensione,supporto,prob,current,giocatoreuno,probabilita,f,&controlloswap,path,blind);
			if(ai==2)
				temporaneo=min_max_green();
			if(temporaneo.x==-1){
				mossa_swap();
				ccc++;
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				return;
			}
			metti_pedina_verde(temporaneo.x, temporaneo.y);
			if(vincita==1){
				aggiorna_pagina();
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL COMPUTER!!!",finestra*0.10,finestra*0.9,blended);
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				vincitore=2;
				vinto();
				return;
			}
			time(&end);
			printf("il computer ci ha messo %d secondi per fare la mossa\n",(int)difftime(end,start)   );
			SDL_UpdateRect(screen, 0, 0, 0, 0);
		}
		else{//rosso
			time(&start);
			IntPoint2 temporaneo;
			if(ai==1)
				temporaneo= mossa_migliore(dimensione,supporto,prob,current,giocatoreuno,probabilita,f,&controlloswap,path,blind);
			if(ai==2)
				temporaneo=min_max_red();
			if(temporaneo.x==-1){
				mossa_swap();
				ccc++;
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				return;
			}
			metti_pedina_rossa(temporaneo.x, temporaneo.y);
			if(vincita==1){
				aggiorna_pagina();
				GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"HA VINTO IL COMPUTER!!!",finestra*0.10,finestra*0.9,blended);
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				vincitore=2;
				vinto();
				return;
			}
			time(&end);
			printf("il computer ci ha messo %d secondi per fare la mossa\n",(int)difftime(end,start)   );
			SDL_UpdateRect(screen, 0, 0, 0, 0);
		}
	}
	else{
		time(&start);
		int asd=1;
		while(asd){
			if(f==1 && ccc==1){
				swap();
				ccc++;
				asd=0;
			}
			if (SDL_PollEvent(&event)){
				if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
					mouse.x=event.button.x;
					mouse.y=event.button.y;
					if(exit_reset()==1)
						continue;
					
					asd=hum();
					time(&end);
					printf("il giocatore ci ha messo %d secondi per fare la mossa\n",(int)difftime(end,start)   );
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				else if (event.type == SDL_KEYDOWN){
					if(tastiera()==1)
						return;
				}
			}
		}
		
	}
}

void gioco(){
	forza_orologio();
	while(1){
		// decidere il giocatore inziale
		orolo();
		if(f==0 && ccc==0){
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			giocatore_iniziale();
			orolo();
		}
			
		// swap mossa
		if(f==1 && players==0 && ccc==1){
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			swap();
			ccc++;
			SDL_UpdateRect(screen, 0, 0, 0, 0);
			orolo();
		}
		if (players==1){
			pc_hum();
		}
		
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				if(exit_reset()==1)
					continue;
				hum();
				orolo();
				SDL_UpdateRect(screen, 0, 0, 0, 0);
			}
			else if (event.type == SDL_KEYDOWN){
				if(tastiera()==1)
					return;
			}
		}
	}
}

//schermate


void scritte_dimensione(){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"di che dimensione vuoi la scacchiera?",finestra/4+5,finestra/8+7,blended);
	
	// fa vedere i corrispondenti tasti per la tastiera
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",200+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",250+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"3",300+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"4",350+5,finestra/8*2-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"5",200+5,finestra/8*3-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"6",250+5,finestra/8*3-23,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"7",300+5,finestra/8*3-23,blended);
	
	GC_DrawRect(screen,200,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,250,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,300,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,350,finestra/8*2,30,30,colori.rectangle);
	GC_DrawRect(screen,200,finestra/8*3,30,30,colori.rectangle);
	GC_DrawRect(screen,250,finestra/8*3,30,30,colori.rectangle);
	GC_DrawRect(screen,300,finestra/8*3,30,30,colori.rectangle);
	
}

void sceltadimensione(){
	scritte_dimensione();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if(event.motion.x>200 && event.motion.x<230 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>250 && event.motion.x<280 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>300 && event.motion.x<330 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>350 && event.motion.x<380 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>200 && event.motion.x<230 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>250 && event.motion.x<280 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>300 && event.motion.x<330 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>50 && event.motion.x<110 && event.motion.y>finestra/8 && event.motion.y<finestra/8+30){
					scritte_dimensione();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"8",200+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"9",250+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"10",300+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"11",350+5,finestra/8*2+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"12",200+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"13",250+5,finestra/8*3+2,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"14",300+5,finestra/8*3+2,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1 ){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				// dimensione8
				if(mouse.x>200 && mouse.x<230 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					dimensione=8;
					scelta_giocatori();
					return;
				}
				//dimensione9
				if(mouse.x>250 && mouse.x<280 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					dimensione=9;
					scelta_giocatori();
					return;
				}
				//dimensione10
				if(mouse.x>300 && mouse.x<330 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					dimensione=10;
					scelta_giocatori();
					return;
				}
				//dimensione11
				if(mouse.x>350 && mouse.x<380 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+30){
					dimensione=11;
					scelta_giocatori();
					return;
				}
				//dimensione12
				if(mouse.x>200 && mouse.x<230 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+30){
					dimensione=12;
					scelta_giocatori();
					return;
				}
				//dimensione13
				if(mouse.x>250 && mouse.x<280 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+30){
					dimensione=13;
					scelta_giocatori();
					return;
				}
				//dimensione14
				if(mouse.x>300 && mouse.x<330 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+30){
					dimensione=14;
					scelta_giocatori();
					return;
				}
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			
			else if(event.type == SDL_KEYDOWN){
				escape();
				// dimensione8
				if(event.key.keysym.sym==SDLK_1){
					dimensione=8;
					scelta_giocatori();
					return;
				}
				//dimensione9
				if(event.key.keysym.sym==SDLK_2){
					dimensione=9;
					scelta_giocatori();
					return;
				}
				//dimensione10
				if(event.key.keysym.sym==SDLK_3){
					dimensione=10;
					scelta_giocatori();
					return;
				}
				//dimensione11
				if(event.key.keysym.sym==SDLK_4){
					dimensione=11;
					scelta_giocatori();
					return;
				}
				//dimensione12
				if(event.key.keysym.sym==SDLK_5){
					dimensione=12;
					scelta_giocatori();
					return;
				}
				//dimensione13
				if(event.key.keysym.sym==SDLK_6){
					dimensione=13;
					scelta_giocatori();
					return;
				}
				//dimensione14
				if(event.key.keysym.sym==SDLK_7){
					dimensione=14;
					scelta_giocatori();
					return;
				}
			}
		}
	}
}

void regole(){
	blank();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"REGOLE",finestra/3,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Ogni giocatore ha un colore (Bianco o Nero). I giocatori mettono a turno",10,finestra/8,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"una pedina del loro colore su una cella esagonale della scacchiera.",10,finestra/8+finestra/15,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"L'obiettivo e' formare una catena delle proprie pedine che congiunga i ",10,finestra/8+finestra/15*2,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"due lati della scacchiera marcati del proprio colore, prima che l'avversario",10,finestra/8+finestra/15*3,blended);
	
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"connetta i suoi lati in maniera simile. Il primo giocatore a fare la ",10,finestra/8+finestra/15*4,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"connessione vince il gioco. Le 4 caselle esagonali ai lati appartengono",10,finestra/8+finestra/15*5,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"ad entrambi i lati. Poiche' il primo giocatore a muovere e'avvantaggiato,",10,finestra/8+finestra/15*6,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"viene applicata la regola della torta, che permette al secondo giocatore ",10,finestra/8+finestra/15*7,blended);
	
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0," di scegliere se scambiare la posizione con il primo giocatore dopo che",10,finestra/8+finestra/15*8,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"questi ha fatto la prima mossa. In pratica il secondo , se vuole avvalersi",10,finestra/8+finestra/15*9,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"di questa possibilita', sostituisce la pedina giocata dal primo giocatore",10,finestra/8+finestra/15*10,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"con la sua, e lascia il turno all'avversario.",10,finestra/8+finestra/15*11,blended);
	
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1 ){
			mouse.x=event.button.x;
			mouse.y=event.button.y;
			// tasto esci
			if (mouse.x<35 && mouse.y<21){
				esci();
			}
			// Menu
			if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
				scelta();
				return;
			}
		}
	}
}

void scritte_scelta_ai(){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"che ai deve usare il computer?",finestra/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",finestra/3-20,finestra/8*2+7,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",finestra/3-20,finestra/8*3+7,blended);
	GC_DrawRect(screen,finestra/3,finestra/8*2  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(screen,finestra/3,finestra/8*3  ,finestra/3,40,colori.rectangle);
}

void scelta_ai(){
	scritte_scelta_ai();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Y-reduction",finestra/3+5,finestra/8*2+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max",finestra/3+5,finestra/8*3+7,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+40){
					scritte_scelta_ai();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Y-reduction",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"min-max",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+40){
					scritte_scelta_ai();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Y-reduction",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"min-max",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+40){
					ai=1;
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+40){
					ai=2;
					rec_level_pc_1();
					return;
				}
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			else if(event.type == SDL_KEYDOWN){
				escape();
				if(event.key.keysym.sym==SDLK_1){
					ai=1;
					return;
				}
				if(event.key.keysym.sym==SDLK_2){
					ai=2;
					rec_level_pc_1();
					return;
				}
			}
		}
	}
}

void scritte_giocatore_iniziale(){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",finestra/3-20,finestra/8+7,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",finestra/3-20,finestra/8*2+7,blended);
	
	GC_DrawRect(screen,finestra/3,finestra/8  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(screen,finestra/3,finestra/4  ,finestra/3,40,colori.rectangle);
	
}

void scelta_giocatore_iniziale(){
	scritte_giocatore_iniziale();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizia il pc",finestra/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizio io",finestra/3+5,finestra/8*2+7,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8 && event.motion.y<finestra/8+40){
					scritte_giocatore_iniziale();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Inizia il pc",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizio io",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*2 && event.motion.y<finestra/8*2+40){
					scritte_giocatore_iniziale();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizia il pc",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Inizio io",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if(event.motion.x>50 && event.motion.x<110 && event.motion.y>finestra/8 && event.motion.y<finestra/8+30){
					scritte_giocatore_iniziale();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizia il pc",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Inizio io",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8 && mouse.y<finestra/8+40){
					giocatoreiniziale1=0;
					scelta_ai();
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+40){
					giocatoreiniziale1=1;
					scelta_ai();
					return;
				}
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			else if(event.type == SDL_KEYDOWN){
				escape();
				if(event.key.keysym.sym==SDLK_1){
					giocatoreiniziale1=0;
					scelta_ai();
					return;
				}
				if(event.key.keysym.sym==SDLK_2){
					giocatoreiniziale1=1;
					scelta_ai();
					return;
				}
			}
		}
	}
}

//1 rosso, 2 verde
void scritte_ai(int a){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	if(a==1)
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"che ai deve usare il rosso?",finestra/3+5,finestra/8+7,blended);
	if(a==2)
		GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"che ai deve usare il verde?",finestra/3+5,finestra/8+7,blended);
	
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",finestra/3-20,finestra/8*2+7,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",finestra/3-20,finestra/8*3+7,blended);
	
	GC_DrawRect(screen,finestra/3,finestra/8*2  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(screen,finestra/3,finestra/8*3  ,finestra/3,40,colori.rectangle);
	
}

void scritte_scelta_giocatori(){
	blank();
	GC_DrawText(screen,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(screen,0,0,35,21,colori.rectangle);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(screen,finestra-60,0,60,21,colori.rectangle);
	
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"1",finestra/3-20,finestra/8+7,blended);
	GC_DrawText(screen,font,0,0,255,0,0,0,0,0,"2",finestra/3-20,finestra/8*2+7,blended);
	
	GC_DrawRect(screen,finestra/3,finestra/8  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(screen,finestra/3,finestra/4  ,finestra/3,40,colori.rectangle);
}

void scelta_giocatori(){
	scritte_scelta_giocatori();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Umano",finestra/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Pc",finestra/3+5,finestra/8*2+7,blended);
	
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8 && event.motion.y<finestra/8+40){
					scritte_scelta_giocatori();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Umano-Umano",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Pc",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/4 && event.motion.y<finestra/4+40){
					scritte_scelta_giocatori();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Umano",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Umano-Pc",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
				if(event.motion.x>50 && event.motion.x<110 && event.motion.y>finestra/8 && event.motion.y<finestra/8+30){
					scritte_scelta_giocatori();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Umano",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Umano-Pc",finestra/3+5,finestra/8*2+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8 && mouse.y<finestra/8+40){
					players=0;
					return;
				}
				if (mouse.x>finestra/3 && mouse.x<finestra/3*2 && mouse.y>finestra/8*2 && mouse.y<finestra/8*2+40){
					players=1;
					scelta_giocatore_iniziale();
					return;
				}
				
				// Menu
				if (mouse.x<finestra && mouse.x>finestra-60 && mouse.y<21){
					scelta();
					return;
				}
			}
			else if(event.type == SDL_KEYDOWN){
				escape();
				if(event.key.keysym.sym==SDLK_1){
					players=0;
					
					return;
				}
				if(event.key.keysym.sym==SDLK_2){
					players=1;
					scelta_giocatore_iniziale();
					return;
				}
				
			}
		}
	}
}


void schermata_iniziale(){
	blank();
	SDL_BlitSurface(spiral,NULL,screen,NULL);
}

void scelta(){
	schermata_iniziale();
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Configura",finestra/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"GIOCA!",finestra*2/3+5,finestra/8+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Continua partita",finestra/3+5,finestra/8*2+7,blended);
	GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Regole",finestra/3+5,finestra/8*3+7,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while(1){
		if (SDL_PollEvent(&event)){
			if(event.type == SDL_MOUSEMOTION){
				// tasto Configura
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8 && event.motion.y<finestra/8+40){
					schermata_iniziale();
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Configura",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"GIOCA!",finestra*2/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Continua partita",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Regole",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				// tasto GIOCA!
				if (event.motion.x>finestra*2/3 && event.motion.x<finestra*2/3+finestra/6 && event.motion.y>finestra/8 && event.motion.y<finestra/8+40){
					schermata_iniziale();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Configura",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"GIOCA!",finestra*2/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Continua partita",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Regole",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				// tasto carica partita
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/4 && event.motion.y<finestra/4+40){
					schermata_iniziale();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Configura",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"GIOCA!",finestra*2/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Continua partita",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Regole",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				// tasto Regole
				if (event.motion.x>finestra/3 && event.motion.x<finestra/3+finestra/4 && event.motion.y>finestra/8*3 && event.motion.y<finestra/8*3+40){
					schermata_iniziale();
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Configura",finestra/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,0,255,0,0,0,0,0,0,"GIOCA!",finestra*2/3+5,finestra/8+7,blended);
					GC_DrawText(screen,font,255,255,255,0,0,0,0,0,"Continua partita",finestra/3+5,finestra/8*2+7,blended);
					GC_DrawText(screen,font,255,0,0,0,0,0,0,0,"Regole",finestra/3+5,finestra/8*3+7,blended);
					SDL_UpdateRect(screen, 0, 0, 0, 0);
				}
				
			}
			if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1){
				mouse.x=event.button.x;
				mouse.y=event.button.y;
				// tasto esci
				if (mouse.x<35 && mouse.y<21){
					esci();
				}
				// tasto Configura
				if (mouse.x>finestra/3 && mouse.x<finestra/3+finestra/4 && mouse.y>finestra/8 && mouse.y<finestra/8+40){
					sceltadimensione();
					azzera();
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					gioco();
				}
				// tasto GIOCA!
				if (mouse.x>finestra*2/3 && mouse.x<finestra*2/3+finestra/6 && mouse.y>finestra/8 && mouse.y<finestra/8+40){
					dimensione=8;
					azzera();
					players=0;
					blank();
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					gioco();
				}
				// tasto carica partita
				if (mouse.x>finestra/3 && mouse.x<finestra/3+finestra/4 && mouse.y>finestra/4 && mouse.y<finestra/4+40){
					load_file();
					return;
				}
				// tasto Regole
				if (mouse.x>finestra/3 && mouse.x<finestra/3+finestra/4 && mouse.y>finestra/8*3 && mouse.y<finestra/8*3+40){
					regole();
				}
				
			}
			else if(event.type == SDL_KEYDOWN){
				if (event.key.keysym.sym==SDLK_0){
					dimensione=8;
					azzera();
					players=0;
					blank();
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					gioco();
				}
				if (event.key.keysym.sym==SDLK_1){
					sceltadimensione();
					azzera();
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					gioco();
				}
				if (event.key.keysym.sym==SDLK_2){
					load_file();
					return;
				}
				if (event.key.keysym.sym==SDLK_3){
					regole();
				}
				
				escape();
			}
		}
	}
}


void riempi_Surface(){
	//fantasy
	GC_DrawText(fantasy,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(fantasy,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(fantasy,0,0,35,21,colori.rectangle);
	GC_DrawText(fantasy,font,255,255,255,0,0,0,0,0,"Menu",finestra-60+3,0,blended);
   	GC_DrawRect(fantasy,finestra-60,0,60,21,colori.rectangle);
	GC_DrawText(fantasy,font,0,0,255,0,0,0,0,0,"s",finestra/3-20,finestra/8+7,blended);
	GC_DrawText(fantasy,font,0,0,255,0,0,0,0,0,"l",finestra/3-20,finestra/8*2+7,blended);
	GC_DrawText(fantasy,font,0,0,255,0,0,0,0,0,"h",finestra/3-20,finestra/8*3+7,blended);
	GC_DrawText(fantasy,font,0,0,255,0,0,0,0,0,"3",finestra/3-20,finestra/8*4+7,blended);
	GC_DrawRect(fantasy,finestra/3,finestra/8  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(fantasy,finestra/3,finestra/4  ,finestra/3,40,colori.rectangle);
	GC_DrawRect(fantasy,finestra/3,finestra*3/8,finestra/3,40,colori.rectangle);
	GC_DrawRect(fantasy,finestra/3,finestra*4/8,finestra/3,40,colori.rectangle);
	
	//spiral
	GC_DrawText(spiral,font2,255,0,0,0,0,0,0,0,"HEX",finestra/2-45,0,blended);
	GC_DrawText(spiral,font,255,0,0,0,0,0,0,0,"Exit",0,0,blended);
	GC_DrawRect(spiral,0,0,35,21,colori.rectangle);
	GC_DrawText(spiral,font,255,0,0,0,0,0,0,0,"0",finestra*2/3-20,finestra/8+7,blended);
	GC_DrawText(spiral,font,255,0,0,0,0,0,0,0,"1",finestra/3-20,finestra/8+7,blended);
	GC_DrawText(spiral,font,255,0,0,0,0,0,0,0,"2",finestra/3-20,finestra/8*2+7,blended);
	GC_DrawText(spiral,font,255,0,0,0,0,0,0,0,"3",finestra/3-20,finestra/8*3+7,blended);
	GC_DrawRect(spiral,finestra*2/3,finestra/8  ,finestra/6,40,colori.rectangle);
	GC_DrawRect(spiral,finestra/3,finestra/8  ,finestra/4,40,colori.rectangle);
	GC_DrawRect(spiral,finestra/3,finestra/4  ,finestra/4,40,colori.rectangle);
	GC_DrawRect(spiral,finestra/3,finestra*3/8,finestra/4,40,colori.rectangle);
	
	float j,i;
	float spostamento=0.7;// diminuendolo l'immagine diventa piu' nitida, ma cosi' e' piu' artistica
	int y=finestra/2 - 17;
	for(i=0;i<finestra;i+=spostamento){
		j=((10*sin(i/7))/(i/finestra)/1.5+y);
		GC_PutPixel(spiral,i,j,colori.red);
	}
	for(i=0;i<finestra;i+=spostamento){
		j=(-(10*sin(i/7))/(i/finestra)/1.5+y);
		GC_PutPixel(spiral,i,j,colori.green);
	}
}


int main(){
	rect.x=0;
	rect.y=0;
	rect.h=finestra;
	rect.w=finestra;
	orologio[2]=':';
	orologio[5]=':';
	VIEWPORT view={0, finestra, 0, finestra};
	
	//librerie SDL e TTF
	
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
		return(1);
	}
	
	screen=SDL_SetVideoMode(view.xmax,view.ymax,24,SDL_HWSURFACE);
	
	if(screen==NULL){
		fprintf(stderr,"Can't set video mode: %s\n",SDL_GetError());
		SDL_Quit();
		return(2);
	}
	img=SDL_CreateRGBSurface(SDL_HWSURFACE,view.xmax,view.ymax,24, 0, 0, 0, 0);
	spiral=SDL_CreateRGBSurface(SDL_HWSURFACE,view.xmax,view.ymax,24, 0, 0, 0, 0);
	fantasy=SDL_CreateRGBSurface(SDL_HWSURFACE,view.xmax,view.ymax,24, 0, 0, 0, 0);
	if(img==NULL){
		fprintf(stderr,"Can't set video mode: %s\n",SDL_GetError());
		SDL_Quit();
		return(2);
	}
	if(spiral==NULL){
		fprintf(stderr,"Can't set video mode: %s\n",SDL_GetError());
		SDL_Quit();
		return(2);
	}
	if(fantasy==NULL){
		fprintf(stderr,"Can't set video mode: %s\n",SDL_GetError());
		SDL_Quit();
		return(2);
	}
	if(TTF_Init () < 0){
		fprintf(stderr, "couldn't initialize TTF : %s\n", SDL_GetError());
		SDL_Quit();
		return(2);
	}
	font= TTF_OpenFont("Images/FreeSans.ttf", 18);
	font2= TTF_OpenFont("Images/FreeSans.ttf", 50);
	
	if (font == NULL)
		fprintf(stderr, "couldn't load font\n");
	if (font2 == NULL)
		fprintf(stderr, "couldn't load font\n");
	
	GC_DrawText(screen,font2,255,255,255,0,0,0,0,0,"LOADING",finestra/2-100,finestra/4,blended);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	
	//screens
	scala_immagine(spiral, finestra, "Images/spiral.jpg");
	scala_immagine(img, finestra, "Images/images.jpg");
	scala_immagine(fantasy, finestra, "Images/sfondo_s.jpg");
	inizializza_colori();
	riempi_Surface();
	f=0;
	vincita=0;
	scelta();
	
	
	SDL_FreeSurface(screen);
	SDL_FreeSurface(img);
	SDL_FreeSurface(spiral);
	SDL_FreeSurface(fantasy);
	TTF_CloseFont(font);
	TTF_CloseFont(font2);
	TTF_Quit();
	SDL_Quit();
	return(0);
}

