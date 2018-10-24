#include <stdio.h>
#include <stdlib.h>
#include "../Libraries/GCVec2dLib.h"
#include "min_max.h"
// giocatore: 1 rosso, 2 verde

int nodi_esaminati;
int nodi_terminali;
int numero_potature;
int nodi_vincenti;
int nodi_perdenti;


int controllo_nodi;//=0 se non si deve tagliare
int prof_finale;
float min_value;

IntPoint2 min_max(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind,int f, int controllo_swap, int conta, int *indici){
	int i,j;
	prof_finale=depth;
	nodi_esaminati=nodi_terminali=numero_potature=nodi_vincenti=nodi_perdenti=0;
	int alpha=-40;
	int beta=40;
	
	min_value=20;
	
	IntPoint2 migliore;
	double happy=-1000;
	double temp;
	int controllo=0;
	equi *equiprobabili;
	equiprobabili=malloc(dimensione*dimensione*sizeof(equi));
	int counter_equi=1;
	if(player==1){//calcolata sul rosso
		if(f==1 && controllo_swap==0){//calcola la bontà della mossa swap
			for(i=0;i<dimensione;i++){
				for(j=0;j<dimensione;j++){
					if(board[i][j]==2){
						board[j][i]=1;
						board[i][j]=0;
						happy=min_max_rosso(board, depth-1,2,prob,dimensione, supporto,path,blind,alpha,beta,conta,indici);
						board[j][i]=0;
						board[i][j]=2;
					}
				}
			}
		}
		for(i=0;i<dimensione;i++){//min_max profondità depth-1
			for(j=0;j<dimensione;j++){
				if(board[i][j]==0){
					board[i][j]=1;
					if (win_rosso(path,blind,board,dimensione)==1){
						migliore.x=i;
						migliore.y=j;
						printf("Il computer ha trovato una combinazione vincente\n");
						return(migliore);
					}
					board[i][j]=2;
					if (win_verde(path,blind,board,dimensione)==1){
						migliore.x=i;
						migliore.y=j;
						nodi_vincenti++;
						printf("Il computer ha bloccato la vincita avversaria\n");
						return(migliore);
					}
					board[i][j]=1;
					controllo_nodi=0;
					temp=min_max_rosso(board, depth-1,2,prob,dimensione, supporto,path,blind, alpha,beta,conta,indici);
					
					//printf("rosso calcolata su %d %d il risultato viene %f \n",i,j,temp);
					
					if(happy<=temp){
						if(happy==temp){
							equiprobabili[counter_equi].x=i;
							equiprobabili[counter_equi].y=j;
							counter_equi++;
						}
						else{
							happy=temp;
							migliore.x=i;
							migliore.y=j;
							controllo=1;
							equiprobabili[0].x=i;
							equiprobabili[0].y=j;
							counter_equi=1;
						}
					}
					board[i][j]=0;
				}
			}
		}
	}
	if(player==2){// calcolata sul verde
		if(f==1 && controllo_swap==0){
			for(i=0;i<dimensione;i++){
				for(j=0;j<dimensione;j++){
					if(board[i][j]==1){
						board[j][i]=2;
						board[i][j]=0;
						happy=min_max_verde(board, depth-1,1,prob,dimensione, supporto,path,blind,alpha,beta,conta,indici);
						board[j][i]=0;
						board[i][j]=1;
					}
				}
			}
		}
		for(i=0;i<dimensione;i++){
			for(j=0;j<dimensione;j++){
				if(board[i][j]==0){
					board[i][j]=2;
					if (win_verde(path,blind,board,dimensione)==1){
						migliore.x=i;
						migliore.y=j;
						printf("Il computer ha bloccato la vincita avversaria\n");
						return(migliore);
					}
					board[i][j]=1;
					if (win_rosso(path,blind,board,dimensione)==1){
						migliore.x=i;
						migliore.y=j;
						printf("Il computer ha bloccato la vincita avversaria\n");
						return(migliore);
					}
					board[i][j]=2;
					
					controllo_nodi=0;
					temp=min_max_verde(board, depth-1,1,prob,dimensione, supporto,path,blind, alpha,beta,conta,indici);
					//printf("verde calcolata su %d %d il risultato viene %f \n",i,j,temp);
					if(happy<=temp){
						if(happy==temp){
							equiprobabili[counter_equi].x=i;
							equiprobabili[counter_equi].y=j;
							counter_equi++;
						}
						else{
							happy=temp;
							migliore.x=i;
							migliore.y=j;
							controllo=1;
							equiprobabili[0].x=i;
							equiprobabili[0].y=j;
							counter_equi=1;
						}
					}
					board[i][j]=0;
				}
			}
		}
	}
	controllo_nodi=0;
	if( controllo==0){
		printf("il pc ha swappato\n");
		migliore.x=-1;
		migliore.y=-1;
	}
	
	else{
		//printf("numero di mosse valide %d\n",counter_equi);
		int k;
		if(depth>1){
			for(k=depth-2;k>=0 && counter_equi!=1;k--){
				for(i=0;i<counter_equi;i++){
					board[equiprobabili[i].x][equiprobabili[i].y]= player;
					if(player==1)
						equiprobabili[i].probabil=min_max_rosso(board,k,2,prob,dimensione, supporto,path,blind, alpha,beta,conta,indici);
					else
						equiprobabili[i].probabil=min_max_verde(board,k,1,prob,dimensione, supporto,path,blind,alpha,beta,conta,indici);
					board[equiprobabili[i].x][equiprobabili[i].y]=0;
				}
				happy=-100;
				j=0;
				for(i=0;i<counter_equi;i++){
					if(happy<=equiprobabili[i].probabil){
						if(happy<equiprobabili[i].probabil){
							happy=equiprobabili[i].probabil;
							j=0;
							equiprobabili[0].x=equiprobabili[i].x;
							equiprobabili[0].y=equiprobabili[i].y;
							equiprobabili[0].probabil=equiprobabili[i].probabil;
						}
						else{
							j++;
							equiprobabili[j].x=equiprobabili[i].x;
							equiprobabili[j].y=equiprobabili[i].y;
						}
					}
				}
				counter_equi=j+1;
				//printf("numero di mosse valide a profondità %d: %d\n",k, counter_equi);
			}
			
			//controlla la vincita avversaria, cosi' facendo sceglie la mossa per bloccare l'avversario
			for(k=depth-1;k>0 && counter_equi!=1;k--){
				//printf("il counter prima di controllare l'avversario:%d\n",counter_equi);
				if(counter_equi !=1 ){
					happy=-1000;
					for(i=0;i<counter_equi;i++){
						if(player==1){
							board[equiprobabili[i].x][equiprobabili[i].y]= 2;
							equiprobabili[i].probabil=min_max_verde(board,k,1,prob,dimensione, supporto,path,blind, alpha,beta,conta,indici);
						}
						else{
							board[equiprobabili[i].x][equiprobabili[i].y]= 1;
							equiprobabili[i].probabil=min_max_rosso(board,k,2,prob,dimensione, supporto,path,blind,alpha,beta,conta,indici);
						}
						board[equiprobabili[i].x][equiprobabili[i].y]=0;
					}
					j=0;
					for(i=0;i<counter_equi;i++){
						if(happy<=equiprobabili[i].probabil){
							if(happy<equiprobabili[i].probabil){
								happy=equiprobabili[i].probabil;
								j=0;
								equiprobabili[0].x=equiprobabili[i].x;
								equiprobabili[0].y=equiprobabili[i].y;
								equiprobabili[0].probabil=equiprobabili[i].probabil;
							}
							else{
								j++;
								equiprobabili[j].x=equiprobabili[i].x;
								equiprobabili[j].y=equiprobabili[i].y;
							}
						}
					}
					counter_equi=j+1;
				}
				//printf("il counter dopo aver controllato l'avversario:%d\n",counter_equi);
			}
		}
		
		
		migliore.x=equiprobabili[0].x;
		migliore.y=equiprobabili[0].y;
		// calcola la mossa piu' probabile
	}
	printf("la mossa migliore e' %d %d , con felicita %f su 1\n", migliore.x,migliore.y, happy);
	
	return(migliore);
}




double min_max_verde(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind, int alpha, int beta, int conta, int *indici){
	int i,j;
	double bestvnodo;
	if (win_rosso(path,blind,board,dimensione)==1){
		//printf("la vincita verde ( ha vinto il rosso) ritorna %d con profondita %d\n",-10-depth, depth);
		return (+10+depth);
	}
	if (win_verde(path,blind,board,dimensione)==1){
		//printf("la vincita verde (ha vinto il verde ) ritorna %d con profondita %d\n",10+depth, depth);
		return (-10-depth);
	}
	if (depth == 0){
		return (-valutazione(dimensione,prob, board, supporto,conta,indici));
	}
	//rosso-avversario
	if (player==1){
		bestvnodo = +50;
		for (i=0;i<dimensione && controllo_nodi==0;i++){
			for (j=0;j<dimensione && controllo_nodi==0;j++){
				if(board[i][j]==0){
					board[i][j]=1;
					bestvnodo = min(bestvnodo, min_max_verde(board, depth-1,2,prob, dimensione, supporto,path,blind,alpha,beta,conta,indici));
					board[i][j]=0;
					beta=bestvnodo;
					if(depth == prof_finale -1){
						if(min_value==20){
							min_value=beta;
						}
						else if(beta<=min_value){
							min_value=beta;
							controllo_nodi=1;
							return(bestvnodo);
						}
					}
					if(beta<alpha){
						return (bestvnodo);
					}
				}
			}
		}
	}
	else //verde, giocatore di turno
	{
		bestvnodo = -50;
		for (i=0;i<dimensione && controllo_nodi==0;i++){
			for (j=0;j<dimensione && controllo_nodi==0;j++){
				if(board[i][j]==0){
					board[i][j]=2;
					bestvnodo = max(bestvnodo, min_max_verde(board, depth-1, 1,prob, dimensione, supporto,path,blind,alpha,beta,conta,indici));
					board[i][j]=0;
					alpha=bestvnodo;
					if(beta<alpha){
						return (bestvnodo);
					}
				}
			}
		}
	}
	return(bestvnodo);
}


double min_max_rosso(int **board, int depth, int player, double *prob, int dimensione, int *supporto,int **path, int **blind, int alpha, int beta, int conta, int *indici){
	int i,j;
	double bestvnodo;
	if (win_rosso(path,blind,board,dimensione)==1){
		//printf("la vincita verde ( ha vinto il rosso) ritorna %d con profondita %d\n",10+depth, depth);
		return (+10+depth);
	}
	if (win_verde(path,blind,board,dimensione)==1){
		//printf("la vincita verde (ha vinto il verde ) ritorna %d con profondita %d\n",-10-depth, depth);
		return (-10-depth);
	}
	
	if (depth == 0){
		return (valutazione(dimensione,prob, board, supporto,conta,indici));
	}
	//verde-avversario
	if (player==2){
		bestvnodo = +50;
		for (i=0;i<dimensione && controllo_nodi==0;i++){
			for (j=0;j<dimensione && controllo_nodi==0;j++){
				if(board[i][j]==0){
					board[i][j]=2;
					bestvnodo = min(bestvnodo, min_max_rosso(board, depth-1,1,prob, dimensione, supporto,path,blind, alpha,beta,conta,indici));
					board[i][j]=0;
					beta=bestvnodo;
					if(depth == prof_finale -1){
						if(min_value==20){
							min_value=beta;
						}
						else if(beta<=min_value){
							min_value=beta;
							controllo_nodi=1;
							return(bestvnodo);
						}
					}
					if(beta<alpha){
						return (bestvnodo);
					}
				}
			}
		}
	}
	else //rosso, giocatore di turno
	{
		bestvnodo = -50;
		for (i=0;i<dimensione && controllo_nodi==0;i++){
			for (j=0;j<dimensione && controllo_nodi==0;j++){
				if(board[i][j]==0){
					board[i][j]=1;
					bestvnodo = max(bestvnodo, min_max_rosso(board, depth-1, 2,prob, dimensione, supporto,path,blind, alpha,beta,conta,indici));
					board[i][j]=0;
					alpha=bestvnodo;
					if(beta<alpha){
						return (bestvnodo);
					}
				}
			}
		}
	}
	return(bestvnodo);
}



double reduction1(int dimensione, int *supporto,double *prob, int **current, int conta, int *indici){
	int i,j;
	
	for(j=0;j<2*dimensione-1;j++){
		i=supporto[j];
		int tmp;
		for(tmp=0;tmp<2*dimensione-1-j;tmp++){
			if (j>=dimensione)
				prob[i]=1;
			if (j<dimensione&& tmp>=dimensione)
				prob[i]=-1;
				
			if(j<dimensione &&tmp<dimensione){
				if(current[dimensione-tmp-1][dimensione-j-1]==1)
					prob[i]=1;
				if(current[dimensione-tmp-1][dimensione-j-1]==2)
					prob[i]=-1;
				if(current[dimensione-tmp-1][dimensione-j-1]==0)
					prob[i]=0;
			}
			i=i+tmp+j+1;
		}
	}
	double a=calcola_y1(dimensione, supporto,prob,conta,indici);
	//printf("%f\n",a);
	return(a);
}


double calcola_y1(int dimensione, int *supporto,double *prob, int conta, int *indici){
	int i;
	double p1,p2,p3;
	for (i=0; i<conta; i=i+3){
		p1=prob[indici[i]];
		p2=prob[indici[i+1]];
		p3=prob[indici[i+2]];
		prob[indici[i]]=0.5*(p1+p2+p3-p1*p2*p3);
		//printf("%d %d in %d\n",indici[i+1],indici[i+2],indici[i]);
	}
	return(prob[0]);
}

double valutazione(int dimensione,double *prob, int **current, int *supporto, int conta, int *indici){
	return(reduction1(dimensione, supporto, prob,current,conta,indici));

}



//controllo della vincita, per migliorare le prestazioni dell'algoritmo




int contr_vitt;


void empty_array(int **arr,int dimensione){
	int i,j;
	for (i=0; i<dimensione; i++)
		for (j=0; j<dimensione; j++)
			arr[i][j]=0;
}

void rosso(int m, int n,int **path,int **blind,int**board, int dimensione, int h){
	path[m][n]=1;
	//printf("sono nella posizione %d %d\n",m,n);
	if(m>0 && m<dimensione && n<dimensione){
		if (board[m][n] == board[m-1][n] && path[m-1][n]==0 && blind[m-1][n]==0){
			rosso(m-1,n,path,blind,board,dimensione,h);
			return;
		}
	}
	if(m>0 && m<dimensione && n<dimensione-1){
		if (board[m][n] == board[m-1][n+1] && path[m-1][n+1]==0 && blind[m-1][n+1]==0){
			if (n+2== dimensione){
				contr_vitt=1;
				path[m-1][n+1]=1;
				return;
			}
			else{
				rosso(m-1,n+1,path,blind,board,dimensione,h);
				return;
			}
		}
	}
	if( n<dimensione-1 && m<dimensione){
		if( board[m][n] == board[m][n+1] && path[m][n+1]==0 && blind[m][n+1]==0) {
			if (n+2== dimensione){
				contr_vitt=1;
				path[m][n+1]=1;
				return;
			}
			else{
				rosso(m,n+1,path,blind,board,dimensione,h);
				return;
			}
	}
	}
	if(m<dimensione-1 && n<dimensione){
		if (board[m][n] == board[m+1][n] && path[m+1][n]==0 && blind[m+1][n]==0){
			rosso(m+1,n,path,blind,board,dimensione,h);
			return;
		}
	}
	if( n>0 && m<dimensione-1 && n<dimensione){
		if (board[m][n] == board[m+1][n-1] && path[m+1][n-1]==0 && blind[m+1][n-1]==0){
			rosso(m+1,n-1,path,blind,board,dimensione,h);
			return;
		}
	}
	if(n>0 && n<dimensione && m<dimensione){
		if (board[m][n] == board[m][n-1] && path[m][n-1]==0 && blind[m][n-1]==0){
			rosso(m,n-1,path,blind,board,dimensione,h);
			return;
		}
	}
	if(contr_vitt!=1){
		blind[m][n]=1;
		empty_array(path,dimensione);
		if (blind[h][0] != 1)
			rosso(h,0,path,blind,board,dimensione,h);
	}
	return;
}

void verde(int m, int n,int **path,int **blind,int**board, int dimensione,int h){
	path[m][n]=1;
	if( m>0 && m<dimensione && n<dimensione){
		if (board[m][n] == board[m-1][n] && path[m-1][n]==0 && blind[m-1][n]==0 ){
				verde(m-1,n,path,blind,board,dimensione,h);
				return;
		}
	}
	if( m>0 && m<dimensione && n<dimensione-1){
		if (board[m][n] == board[m-1][n+1] && path[m-1][n+1]==0 && blind[m-1][n+1]==0){
				verde(m-1,n+1,path,blind,board,dimensione,h);
				return;
		}
	}
	if( n<dimensione-1 && m<dimensione){
		if (board[m][n] == board[m][n+1] && path[m][n+1]==0 && blind[m][n+1]==0){
				verde(m,n+1,path,blind,board,dimensione,h);
				return;
		}
	}
	if( n<dimensione && m<dimensione-1){
		if (board[m][n] == board[m+1][n] && path[m+1][n]==0 && blind[m+1][n]==0){
			if (m+2== dimensione){
				contr_vitt=1;
				path[m+1][n]=1;
				return;
			}
			else {
				verde(m+1,n,path,blind,board,dimensione,h);
				return;
			}
		}
	}
	if(n>0 && m<dimensione-1 && n<dimensione){
		if (board[m][n] == board[m+1][n-1] && path[m+1][n-1]==0 && blind[m+1][n-1]==0){
			if (m+2== dimensione){
				contr_vitt=1;
				path[m+1][n-1]=1;
				return;
			}
			else 
			verde(m+1,n-1,path,blind,board,dimensione,h);
		}
	}
	if( n>0 && n<dimensione && m<dimensione){
		if (board[m][n] == board[m][n-1] && path[m][n-1]==0 && blind[m][n-1]==0){
			verde(m,n-1,path,blind,board,dimensione,h);
			return;
		}
	}
	if(contr_vitt!=1){
		blind[m][n]=1;
		empty_array(path,dimensione);
		if (blind[0][h] != 1) {
			verde(0,h,path,blind,board,dimensione,h);
		}
	}
	return;
}

int win_rosso(int **path,int **blind,int**board, int dimensione){
	int h=0;
	contr_vitt=0;
	empty_array(blind,dimensione);
	empty_array(path,dimensione);
	if (board[0][0] == 1 )
		rosso(0,0,path,blind,board,dimensione,h);
	for (h=1; h<dimensione ; h++){
		if (board[h][0] == 1 ){
			if (board[h-1][0] != board[h][0]){
				rosso(h,0,path,blind,board,dimensione,h);
				if(contr_vitt==1)
					return(1);
			}
		}
	}
	return(0);
}

int win_verde(int **path,int **blind,int**board, int dimensione){
	int h=0;
	contr_vitt=0;
	empty_array(blind,dimensione);
	empty_array(path,dimensione);
	if (board[0][0] == 2 )
		verde(0,0,path,blind,board,dimensione,h);
	for (h=1; h<dimensione; h++)
		if (board[0][h] == 2){
			if (board[0][h-1] != board[0][h]){
				verde(0,h,path,blind,board,dimensione,h);
				if(contr_vitt==1)
					return(1);
			}
		}
	return(0);
}

