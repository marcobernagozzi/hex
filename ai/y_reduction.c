#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Libraries/GCVec2dLib.h"
#include "min_max.h"
#include "y_reduction.h"

void reduction(int dimensione, int *supporto,double *prob, int **current){
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
	//for(i=0;i<(2*dimensione-1)*dimensione;i++)
	//	printf("%d %f\n",i, prob[i]);
}

double calcola_y(int dimensione, int *supporto,double *prob){
	int i,j,t;
	double p1,p2,p3,temp,temp2;
	int counter;
	for(t=0;t<2*dimensione-2;t++){
		for(j=0;j<2*dimensione-2-t;j++){
			counter=0;
			for(i=(2*dimensione-2-t);i>j;i--){
				p1=prob[supporto[i]-j];
				p2=prob[supporto[i]-1-j];
				p3=prob[supporto[i-1]-j];
				//printf("%d\n",supporto[i-1]-j);
				if(i%2==1){
					temp=0.5*(p1+p2+p3-p1*p2*p3);
					if(counter!=0){
						prob[supporto[i]-j]=temp2;
						//printf("ho sostituito in %d temp2=%f\n",supporto[i]-j,temp2);
					}
				}
				else{
					temp2=0.5*(p1+p2+p3-p1*p2*p3);
					if(counter!=0){
						prob[supporto[i]-j]=temp;
						//printf("ho sostituito in %d temp=%f\n",supporto[i]-j,temp);
					}
				}
				if(i==j+1){
					if(i%2==1){
						prob[supporto[i-1]-j]=temp;
						//printf("FINE ho sostituito in %d temp=%f\n",supporto[i-1]-j,temp);
					}
					else{
						prob[supporto[i-1]-j]=temp2;
						//printf("FINE ho sostituito in %d temp2=%f\n",supporto[i-1]-j,temp2);
					}
				}
				counter++;
			}
		}
		//printf("----------------------------------------------------\n");
	}
	return(prob[0]);
}

IntPoint2 mossa_migliore(int dimensione, int *supporto,double *prob, int **current, int giocatoreuno, IntPoint2 *probabilita,int f,int *swap, int **path, int **blind){
	
	int i,j;
	int contatore =0;
	double max;
	if((f%2)==giocatoreuno){
		//printf("ai calcolata sul rosso\n");
		max = -2;
	}
	else{
		//printf("ai calcolata sul verde\n");
		max = 2;
	}
	IntPoint2 best_move;
	
	/*
	reduction(dimensione,supporto,prob,current,giocatoreuno,probabilita);
	for(i=0;i<(2*dimensione-1)*dimensione;i++)
		printf("%d %f %d\n",i, prob[i],f);
	printf("#################################################################\n\n");
	*/
	for(j=0;j<2*dimensione-1;j++){
		i=supporto[j];
		int tmp;
		for(tmp=0;tmp<2*dimensione-1-j;tmp++){
			if(j<dimensione &&tmp<dimensione){
				if(current[dimensione-tmp-1][dimensione-j-1]==0){
					reduction(dimensione,supporto,prob,current);
					//rosso
					if((f%2)==giocatoreuno){
						prob[i]=1;
						calcola_y(dimensione,supporto,prob);
						//printf("probabilita conseguente alla mossa %d viene %f\n",i,prob[0]);
						if(max<=prob[0]){
							if(max<prob[0]){
								contatore=0;
								max=prob[0];
							}
							if(max==prob[0]){
								probabilita[contatore].x=dimensione-tmp-1;
								probabilita[contatore].y=dimensione-j-1;
								//printf("---------%d     %d %d\n",contatore, dimensione-tmp-1,dimensione-j-1);
								contatore++;
							}
						}
					}
					//verde
					if((f%2)!=giocatoreuno){
						prob[i]=-1;
						calcola_y(dimensione,supporto,prob);
						//printf("%f\n",prob[0]);
						if(max>=prob[0]){
							if(max>prob[0]){
								contatore=0;
								max=prob[0];
								//printf("%f\n",max);
							}
							if(max==prob[0]){
								probabilita[contatore].x=dimensione-tmp-1;
								probabilita[contatore].y=dimensione-j-1;
								//printf("---------%d     %d %d\n",contatore, dimensione-tmp-1,dimensione-j-1);
								contatore++;
							}
						}
					}
				}
			}
			i=i+tmp+j+1;
		}
	}
	if(f==1 && *swap==0){
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
					if(current[dimensione-j-1][dimensione-tmp-1]==1)
						prob[i]=-1;
					if(current[dimensione-tmp-1][dimensione-j-1]==2)
						prob[i]=1;
					if(current[dimensione-tmp-1][dimensione-j-1]==0)
						prob[i]=0;
				}
				i=i+tmp+j+1;
			}
		}
		*swap=*swap+1;
		calcola_y(dimensione,supporto,prob);
		if((giocatoreuno==1 && f%2==1)||(giocatoreuno==0&& f%2==0)){
			if(max<=prob[0]){
				printf("il computer ha swappato\n");
				best_move.x=-1;
				best_move.y=-1;
				return(best_move);
			}
		}
		if((giocatoreuno==1 && f%2==0)||(giocatoreuno==0&& f%2==1)){
			if(max>=prob[0]){
				printf("il computer ha swappato\n");
				best_move.x=-1;
				best_move.y=-1;
				return(best_move);
			}
		}
	}
	//selezione migliore per le mosse con valore uguale
	
	
	//printf("------------------------------------------------------------------------------------\n");
	best_move.x=probabilita[0].x;
	best_move.y=probabilita[0].y;
	//printf("ho scelto come mossa: %d %d\n",best_move.x,best_move.y);
	return(best_move);
}



