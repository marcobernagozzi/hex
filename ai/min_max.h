#ifndef __min_max_H__
#define __min_max_H__

typedef struct{
	int x;
	int y;
	float probabil;
} equi;

IntPoint2 min_max(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind,int,int,int,int*);

double min_max_verde(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind,int,int,int,int*);

double min_max_rosso(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind,int,int,int,int*);

double reduction1(int dimensione, int *supporto,double *prob, int **current, int conta,int *indici);

double calcola_y1(int dimensione, int *supporto,double *prob, int conta,int *indici);

double calcola_y2(int dimensione, int *supporto,double *prob, int conta,int *indici);

double valutazione(int dimensione,double *prob, int **current, int *supporto, int conta,int *indici);


void empty_array(int **arr,int dimensione);

void rosso(int m, int n,int **path,int **blind,int**board, int dimensione, int h);

void verde(int m, int n,int **path,int **blind,int**board, int dimensione, int h);

int win_rosso(int **path,int **blind,int**board, int dimensione);

int win_verde(int **path,int **blind,int**board, int dimensione);




#endif