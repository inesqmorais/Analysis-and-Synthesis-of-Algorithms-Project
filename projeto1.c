#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LESS(A,B) ((A) < (B) ? A : B)



/* ------------ ESTRUTURAS -------------*/


typedef struct node {
	int value;
	struct node *next;
}* link;


typedef struct vertex {
	int id;                /* id do vertice*/
	int scc;               /*numero da subrede do vertice*/
	int sccId;             /*id da sub-rede que corresponde ao vertice com menor identificador dessa sub-rede*/
	int discovered;
	int low;
	link node;
	int raiz;              /*raiz da componente fortemente ligada correspondente a esse vertice*/
}* Vertex;


typedef struct graph {
	int numV;
	int numE;
	Vertex *v;              
	int sccNum;            /* numero total de subredes do grafo */
}* Graph;


typedef struct connection {    /*-- estrutura para ligacoes entre subredes --*/
  	int origin;                         
  	int destiny;
} Connection;





/*--------------- PILHA ------------------*/

static struct node *top;


void StackInit() {

	top = NULL;

}


void StackPush(int value) {

 	link new = (link)malloc(sizeof(struct node));
 	new->value= value;
 	new->next = top;
 	top=new;
}


int StackIsEmpty() {

	return top == NULL;
}


int StackPop(){

	int value;
	link old;

	if(!StackIsEmpty()) {
		value = top->value;
		old = top;
		top = top->next;
		free(old);
		return value;
	}

	else
		return -1;
}




/*------------ FUNCOES AUXILIARES -------------*/


link InsertBegin(int value, link head) {              
  link new = (link)malloc(sizeof(struct node)); 
  new->value = value; 
  new->next = head; 
  return new; 
} 


Vertex VertexInit(int iter) {                         /*-- funcao de inicializacao do vertice --*/

  Vertex v = (Vertex)malloc(sizeof(struct vertex));
  v->id = iter + 1;
  v->node = NULL; 
  v->discovered = -1;                                 /* valor de discovered e low do vertice sao inicializados a -1*/
  v->low = -1;                                        /* quando o vertice é inicializado */
  v->scc = 0;
  v->sccId = 0;
  v->raiz = 0;


  return v;
}


Graph GraphInit(int vertices, int edges) {            /*-- funcao de inicializacao do grafo --*/           
	int i; 

  Graph G = (Graph)malloc(sizeof(struct graph));  
  G->numV = vertices;  
  G->numE = edges; 
  G->sccNum = 0;

  G->v = malloc(sizeof(struct vertex)*(vertices));   

  for (i = 0; i < vertices; i++) {                   /* para cada vertice é chamada a funcao de inicializacao do vertice*/
    G->v[i] = VertexInit(i);
  }
  return G; 
}



void GRAPHinsertEdge(Graph G, int vertexOrigin, int vertexDestiny) {       /*funcao de insercao dos arcos na lista ligada de cada vertice do grafo*/

  G->v[vertexOrigin-1]->node = InsertBegin(vertexDestiny, G->v[vertexOrigin-1]->node); 
 
} 


/* Funcao de comparacao para a ordenacao crescente das ligacoes, chamada no qsort. */
int cmpfunc(const void *a, const void *b) {

  Connection* connection1 = (Connection*)a;
  Connection* connection2 = (Connection*)b;


  if(connection1->origin != connection2->origin) {            
    return ( connection1->origin - connection2->origin);

  }

  else {
    return (connection1->destiny - connection2->destiny);

  }

}




/*-------------- ALGORITMO TARJAN -------------*/



void Tarjan_Visit( int iter, Graph g, int* inStack){

  static int visited=0;

  g->v[iter]->discovered = g->v[iter]->low = visited;

  visited++;
  StackPush(g->v[iter]->id);
  inStack[iter] = 1;                     /* Flag fica a 1 para indicar que o vertice esta dentro da pilha */

  link t;
  for(t = g->v[iter]->node; t!=NULL; t=t->next){                           
    if (g->v[t->value -1]->discovered == -1 || inStack[t->value-1] == 1) {  /* t->value-1 porque o vetor de vertices comeca a contar do zero*/

      if(g->v[t->value -1]->discovered == -1){
        Tarjan_Visit( t->value -1 , g,inStack);
      }
      g->v[iter]->low = LESS(g->v[iter]->low, g->v[t->value -1]->low);       /* LESS devolve o menor dos dois numeros de low */


    }

  }

  /* remocao dos elementos da pilha que formam a componente fortemente ligada */
  if(g->v[iter]->discovered == g->v[iter]->low){
    
    int min = g->v[iter]->id;       /* min vai guardar o vertice com o menor identificador da subrede correspondente*/           
    
    int v = StackPop();

    if(min > v) {                   /*atualizacao do minimo*/
      min = v;
    }

    inStack[v-1] = 0;                /*flag fica a O para indicar que o vertice ja nao esta dentro da pilha*/                
    g->v[v-1]->scc = g->sccNum;
    g->v[v-1]->raiz = g->v[iter]->id;   /* atualizacao do valor da raiz da subrede*/
                                    /* apenas o vertice raiz vai conhecer o  id da sub-rede (=min)
                                      por isso guardamos o valor da raiz em todos os vertices dessa subrede*/



    while(g->v[iter]->id != v) {
      v = StackPop();
      
      if(min > v) {                   
        min = v;
      }

      g->v[v-1]->scc = g->sccNum;
      inStack[v -1] = 0; 
      g->v[v-1]->raiz = g->v[iter]->id;

    }
    
    
    g->v[v-1]->sccId = min;          /* atualizacao do id da subrede*/ 
    g->sccNum++;

  }

}



void SCC_Tarjan(Graph g) {
  int i;

  int* inStack = (int*)malloc(sizeof(int)*(g->numV));    /*vetor que indica se o vertice esta na pilha ou nao*/

  StackInit();                 

  for(i=0; i< g->numV; i++) {
    if(g->v[i]->discovered == -1){
      Tarjan_Visit(i,g, inStack);

    }
  }
}







int main(){

	int vertices, edges;
	scanf("%d", &vertices);
	scanf("%d", &edges);
	Graph graph = GraphInit(vertices, edges);

	int i;
	
	int vertexOrigin, vertexDestiny;

	for(i=0; i < edges; i++) {
		scanf("%d %d",&vertexOrigin, &vertexDestiny);                     
		GRAPHinsertEdge(graph, vertexOrigin, vertexDestiny);
	}


	SCC_Tarjan(graph);        /* identificacao das SCC's e do numero de SCC's */



	int numConnections = 0;    /*contador de ligacoes entre subredes do grafo */
  int n = 0;

	Connection *connections =  (Connection*)malloc(sizeof(struct connection)*edges);   /*vetor de ligacoes*/

  	


    link t;
    /*ciclo que percorre os vertices e seus adjacentes para verificar ligacoes entre subredes*/
  	for(i=0; i<vertices; i++) {
      
      for(t = graph->v[i]->node; t!=NULL; t=t->next){

        if(graph->v[i]->scc != graph->v[t->value-1]->scc){   /* condicao para verificar se os vertices adjacentes estao em SCC's diferentes */
            
        	numConnections++;                                  
 
        	int raizOrigem = graph->v[i]->raiz;
          int raizDestino = graph->v[t->value-1]->raiz;

          connections[n].origin = graph->v[raizOrigem-1]->sccId;        /* ligacoes sao guardados no vetor de ligacoes  */    
          connections[n++].destiny = graph->v[raizDestino-1]->sccId;    /* com id da subrede de origem e subrede de destino*/ 
            
        	}
      	}
  	}


  	
    qsort(connections,numConnections,sizeof(struct connection),cmpfunc);  /*ordenacao do vetor de ligacoes*/


  	
    int totalConnections = numConnections;

  	for(i = 1; i<totalConnections; i++) { 

    	if(connections[i].origin == connections[i-1].origin && connections[i].destiny == connections[i-1].destiny) {

      		connections[i-1].origin = -1; /* colocamos ligacoes repetidas a -1 para depois nao serem impressas */
      		connections[i-1].destiny = -1;

      		numConnections--;
    	}

  	}


	printf("%d\n", graph->sccNum);

	printf("%d\n",numConnections);


  	for (i = 0; i < totalConnections; i++) {                  /*impressao das ligacoes*/
    	if( connections[i].origin != -1) 
      		printf("%d %d\n",connections[i].origin, connections[i].destiny);
  	}


	return 0;

}