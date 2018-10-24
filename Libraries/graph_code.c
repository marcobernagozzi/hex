#include <stdio.h>
#include <stdlib.h>
#include "graph_code.h"


/******************************/
/*Funzioni per gestione Queue:*/
/*create_Queue                */
/*free_Queue                  */
/*Queue_isEmpty               */
/*Queue_add                   */
/*Queue_delete                */
/******************************/
void create_Queue(Queue *Q)
{
  Q->first = malloc(sizeof(Node));
  Q->first->next = NULL;
  Q->last = Q->first;
}

void free_Queue(Queue *Q)
{
  free(Q->first);
}

// controlla se la coda e' vuota
int Queue_isEmpty(Queue *Q)
{
  return (Q->first->next == NULL);
}

// aggiunge un elemento alla coda
void Queue_add(int x,Queue *Q)
{
Node *aux;

  aux = malloc(sizeof(Node));
  aux->info = x;
  aux->next = NULL;
  Q->last->next = aux;
  Q->last = aux;
}

// estrae un elemento dalla coda
int Queue_delete(Queue *Q)
{
Node *aux;
int value;

  aux = Q->first->next;
  value = aux->info;
  Q->first->next = aux->next;
  if (Q->last == aux) 
    Q->last = Q->first;
  free(aux);
  return value;
}

/******************************/
/*Funzioni per gestione Graph:*/
/*create_Graph                */
/*free_Graph                  */
/*Graph_isConnected           */
/*Graph_addEdge               */
/*Graph_delEdge               */
/*Graph_delNode               */
/*Graph_isolNode              */
/*Graph_contract              */
/*Graph_print                 */
/*Graph_DFS                   */
/*Graph_BFS                   */
/*Graph_minPath               */
/******************************/
void create_Graph(int size,Graph *g)
{
int i,j;
int n;
int **A;

  if (size < 2)
	n = 2;
  else
	n = size;
 
  A = malloc(n*sizeof(int *));
  for (i = 0; i < n; ++i)
	A[i] = malloc(n*sizeof(int));
 
  for (i = 0; i < n; ++i)
	for (j = 0; j < n; ++j)
	  A[i][j] = 0;

  g->n=n;
  g->A=A; 
}

void free_Graph(Graph *g)
{
int i,n;
int **A;

  A=g->A;
  n=g->n;
  for (i = 0; i < n; ++i)
	free(A[i]);
  free(A);
}

// controlla se i nodi x e y sono collegati
int Graph_isConnected(int x, int y, Graph *g)
{
int **A;

  A=g->A;
  return (A[x-1][y-1] == 1);
}

// aggiunge l'edge (x,y) al grafo
void Graph_addEdge(int x, int y, Graph *g)
{
int **A;

  A=g->A;
  A[x-1][y-1] = A[y-1][x-1] = 1;
}

// rimuove l'edge (x,y) dal grafo
void Graph_delEdge(int x, int y, Graph *g)
{
int **A;

  A=g->A;
  A[x-1][y-1] = A[y-1][x-1] = 0;
}

// rimuove il nodo x dal grafo
void Graph_delNode(int x, Graph *g)
{
int i,j,n;
int **B;
int **A;
 
  A=g->A;
  n=g->n;
  B = malloc((n-1)*sizeof(int *));
  for (i = 0; i < n-1; ++i)
	B[i] = malloc((n-1)*sizeof(int));
 
  for (i=0; i<n; ++i)
  {
	for (j=0; j<n; ++j)
	{
	  if (i<x-1 && j<x-1)
		B[i][j] = A[i][j];
	  else
	  {
		if (i<x-1 && j>x-1)
		  B[i][j-1] = A[i][j];
		else
		{
		  if (i>x-1 && j<x-1)
			B[i-1][j] = A[i][j];
		  else
		  {
			if (i>x-1 && j>x-1)
			  B[i-1][j-1] = A[i][j];
		  }
		}
	  }
	}
  }
 
  for (i=0; i<n; ++i)
	free(A[i]);
  free(A);
 
  g->A = B;
  g->n --;
}

// disconnette il nodo x dai suoi vicini
void Graph_isolNode(int x, Graph *g)
{
int j,n;
int **A;

  A=g->A;
  n=g->n;
  for (j=0; j<n; ++j)
  {
	if (A[x-1][j]==1)
	{
	  A[x-1][j] = A[j][x-1] = 0;
	}
  }
}

// contrae un vertice del grafo:
// lo isola/rimuove dal grafo e introduce un edge 
// tra ogni coppia di suoi vicini
void Graph_contract(int x, Graph *g)
{
int i,j,n;
int neigh_num,*neigh;
int **A;
 
  A=g->A;
  n=g->n;
  // numero dei vicini del vertice da contrarre
  neigh_num = 0;
  for (j=0; j<n; ++j)
  {
	if (A[x-1][j] == 1)
	  neigh_num++;
  }
 
  // vicini del vertice da contrarre
  neigh = malloc(neigh_num*sizeof(int));
  i = 0;
  for (j=0; j<n; ++j)
  {
	if (A[x-1][j] == 1)
	{
	  neigh[i] = j;
	  i++;
	}
  }
 
  for (i=0; i<neigh_num-1; ++i)
  {
	for (j=i+1; j<neigh_num; ++j)
	{
	  A[neigh[i]][neigh[j]] = A[neigh[j]][neigh[i]] = 1;
	}
  }

  Graph_isolNode(x,g); 
  Graph_delNode(x,g);
  free(neigh);
}

// stampa il grafo
// mode = 0 elenco edge, 1 matrice delle connessioni completa, 
// 2 entrambi
void Graph_print(int mode, Graph *g)
{
int i,j;
int n;
int **A;

  n=g->n;
  A=g->A;
 
  printf("\n");
  printf("Graph with %d Nodes\n.",n);
 
  if (mode==0 || mode==2)
  {
	printf("Edges: \n");
	for (i=0; i<n; ++i)
	{
	  for (j=i+1; j<n; ++j)
		if (A[i][j] == 1)
		  printf("%d - %d\n",i+1,j+1);
	}
  }
  if (mode==1 || mode==2)
  {
	printf("Connections matrix: \n");
	for (i=0; i<n; ++i)
	{
	  for (j=0; j<n; ++j)
		printf("%d ",A[i][j]);
	  printf("\n");
	}
  }
  printf("\n");
}

// Depth First Search con nodo di partenza x
void DFS(int x, Graph *g, int *visited, int *k)
{
int i,n;

 // printf("chiamata %d\n",x);
  n=g->n;
  visited[x]=1;
  (*k)++;
  for (i = 1; i <= n; ++i)
     if (Graph_isConnected(x, i, g) && !visited[i])
        DFS(i, g, visited, k);
}

// Funzione di innesco Depth First Search con nodo di partenza x
int Graph_DFS(int x,int y, Graph *g)
{
int *visited;
int i,k=0,n;

  n=g->n;
  visited = malloc((n+1)*sizeof(int));
  for (i = 1; i <= n; ++i)
        visited[i] = 0;

  //printf("Depth First Search starting from vertex \n");
  //printf("%d\n",x);

  //printf("chiamata di innesco\n");
  DFS(x, g, visited, &k);

  //printf("nodi visitati = %d\n",k);
  if (visited[y]==0)
   return(1);
  else
   return(0);// il grafo e connesso
  printf("\n");
  free(visited);
}

// Breadth First Search con nodo di partenza x
void Graph_BFS(int x, Graph *g)
{
Queue Q;
int *visited;
int i,k,n;

  create_Queue(&Q);
  n=g->n;
  visited = malloc((n+1)*sizeof(int));
  for (i = 1; i <= n; ++i)
        visited[i] = 0;

  Queue_add(x,&Q);
  visited[x] = 1;

  //printf("Breadth First Search starting from vertex \n");
  //printf("%d\n",x);

  while (!Queue_isEmpty(&Q))
  {
        k = Queue_delete(&Q);
        //printf("%d ",k);
        for (i = 1; i <= n; ++i)
          if (Graph_isConnected(k, i, g) && !visited[i])
          {
                Queue_add(i,&Q);
                visited[i] = 1;
          }
  }

  //printf("\n");
  free(visited);
}

// determina il percorso piu' breve tra due nodi
int Graph_minPath(int start, int target, Graph *g)
{
Queue Q;
int i, p, q, k;
int found,temp;
struct aux 
{ 
  int current, prev; 
};
typedef struct aux Aux;
Aux *X;
int *Y;
int *visited;
int n;

  create_Queue(&Q);
  n=g->n;
 
  X = malloc((n+1)*sizeof(Aux));;
  Y = malloc((n+1)*sizeof(int));
  visited = malloc((n+1)*sizeof(int));
  for (i = 1; i <= n; ++i)
	visited[i] = 0;
 
  Queue_add(start,&Q);
  visited[start] = 1;
  found = 0;
  p = q = 0;
  X[0].current = start;
  X[0].prev = 0;
 
  while (!Queue_isEmpty(&Q) && !found)
  {
	k = Queue_delete(&Q);
	for (i = 1; i <= n && !found; ++i)
	  if (Graph_isConnected(k, i, g) && !visited[i])
	  {
		Queue_add(i, &Q);
		++q;
		X[q].current = i;
		X[q].prev = p;
		visited[i] = 1;
		if (i == target) found = 1;
	  }
	  ++p;
  }
 
  //printf("The minimum length path from %d",start);
  //printf(" to %d is:\n",target);
  p = 0;
  while (q)
  {
	Y[p] = X[q].current;
	q = X[q].prev;
	++p;
  }
  Y[p] = X[0].current;
  for (q = 0; q <= p/2; ++q)
  {
	temp = Y[q];
	Y[q] = Y[p-q];
	Y[p-q] = temp;
  }
//stampa path minimo
   //for (q = 0; q <= p; ++q)
 	//printf("%d ",Y[q]);
   //printf("\n");
   //printf("Length = %d\n", q-1);
   //printf("Length = %d\n",p);
 
  free(visited);
  free(X);
  free(Y);
  
  return p;
}


