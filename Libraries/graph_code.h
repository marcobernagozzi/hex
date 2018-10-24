#ifndef __graph_code_H__
#define __graph_code_H__
// struttura node e tipo Node
 struct node
{
  int info;
  struct node *next;
};
 typedef struct node Node;

// struttura e tipo Queue
 typedef struct
{
  Node *first, *last;
}Queue;

//struttura e tipo Graph
 typedef struct
{
  int n;
  int **A;
}Graph;

/******************************/
/*Funzioni per gestione Queue:*/
/*create_Queue                */
/*free_Queue                  */
/*Queue_isEmpty               */
/*Queue_add                   */
/*Queue_delete                */
/******************************/
void create_Queue(Queue *Q);

void free_Queue(Queue *Q);

// controlla se la coda e' vuota
int Queue_isEmpty(Queue *Q);

// aggiunge un elemento alla coda
void Queue_add(int x,Queue *Q);

// estrae un elemento dalla coda
int Queue_delete(Queue *Q);

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
void create_Graph(int size,Graph *g);

void free_Graph(Graph *g);

// controlla se i nodi x e y sono collegati
int Graph_isConnected(int x, int y, Graph *g);

// aggiunge l'edge (x,y) al grafo
void Graph_addEdge(int x, int y, Graph *g);

// rimuove l'edge (x,y) dal grafo
void Graph_delEdge(int x, int y, Graph *g);

// rimuove il nodo x dal grafo
void Graph_delNode(int x, Graph *g);

// disconnette il nodo x dai suoi vicini
void Graph_isolNode(int x, Graph *g);

// contrae un vertice del grafo:
// lo isola/rimuove dal grafo e introduce un edge 
// tra ogni coppia di suoi vicini
void Graph_contract(int x, Graph *g);

// stampa il grafo
// mode = 0 elenco edge, 1 matrice delle connessioni completa, 
// 2 entrambi
void Graph_print(int mode, Graph *g);

// Depth First Search con nodo di partenza x
void DFS(int x, Graph *g, int *visited, int *k);

// Funzione di innesco Depth First Search con nodo di partenza x
int Graph_DFS(int x,int y, Graph *g);

// Breadth First Search con nodo di partenza x
void Graph_BFS(int x, Graph *g);

// determina il percorso piu' breve tra due nodi
int Graph_minPath(int start, int target, Graph *g);

#endif
