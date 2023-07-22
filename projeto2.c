#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define min(A,B) ((A) < (B) ? A : B)

#define bool int 
#define true 1
#define false 0
#define C -100
#define P -200




/* ------------ ESTRUTURAS -------------*/

typedef struct nodeQ {
    int value;
    struct nodeQ *next;
} Node;


typedef struct queue {
    Node *front;
    Node *last;
    unsigned int size;
} Queue;

typedef struct node{
    int cap;
    int value;
    int lig;
    int bLig;
    struct node *next;
}* link;

typedef struct vertex {
    int pixel;
    int parent;
    int lig;
    int bLig;
    link adj;
}* Vertex;

typedef struct graph {
    int numV; /* mLinhas*nColunas */
    int totalV; /* mLinhas*nColunas + 2 */
    int numE;
    int mLinhas;
    int nColunas;
    Vertex* v;                        
}* Graph;


int contP=0; /* contador de peso minimo total da segmentacao */


/*----------------------------------------*/




/*----------------GRAFO--------------------*/

Vertex VertexInit() {                        

  Vertex v = (Vertex)malloc(sizeof(struct vertex));
  v->adj = NULL; 
  v->pixel = 0;
  v->parent = -1;
  v->lig = -1;
  v->bLig = -1;


  return v;
}


Graph GRAPHinit(int V, int linhas, int colunas) { 
    int i; 
    Graph G = malloc(sizeof(struct graph));  
    G->totalV = V; 
    G->numV = V-2; 
    G->mLinhas = linhas;
    G->nColunas = colunas; 

    G->v = malloc(sizeof(struct vertex)*(V));

    for (i = 0; i < V; i++) 
        G->v[i] = VertexInit();
    return G; 
}





link InsertBegin(int value, link head, int vert, int lig, int bLig) {              
  link new = (link)malloc(sizeof(struct node)); 
  new->value = vert;
  new->cap = value;
  new->next = head; 
  new->lig = lig; /* identificador da ligacao */
  new->bLig = bLig; /* identificador da ligacao que lhe e' inversa */
  return new; 
}



void GRAPHinsertS(Graph G, int value,int vertice, int cont) { 
    G->v[0]->adj = InsertBegin(value, G->v[0]->adj, vertice, cont, -1); /* nao tem ligacao inversa, dai ser-lhe atribuida "-1" */
} 

void GRAPHinsertT(Graph G, int value, int vertice, int cont) {
    G->v[vertice]->adj = InsertBegin(value, G->v[vertice]->adj, (G->totalV-1), cont, -1); /* nao tem ligacao inversa, dai ser-lhe atribuida "-1" */
}

void GRAPHinsertHorizontalE(Graph G, int value, int cont, int contLig) {
    G->v[cont]->adj= InsertBegin(value, G->v[cont]->adj, cont+1, contLig, contLig + 1);
    G->v[cont+1]->adj= InsertBegin(value,G->v[cont+1]->adj, cont, contLig + 1, contLig);
}

void GRAPHinsertVerticalE(Graph G,int value,int cont, int contLig) {
    G->v[cont]->adj = InsertBegin(value, G->v[cont]->adj, cont+G->nColunas,contLig, contLig + 1);
    G->v[cont+G->nColunas]->adj = InsertBegin(value, G->v[cont+G->nColunas]->adj, cont,contLig + 1, contLig);

}


/*-----------------------------------------*/




/*---------------FILA---------------------*/

void init(Queue *q) {
    q->front = NULL;
    q->last = NULL;
    q->size = 0;
}

 
void pop(Queue *q) {
    q->size--;
 
    Node *tmp = q->front;
    q->front = q->front->next;
    free(tmp);
}
 
void push(Queue *q, int value) {
    q->size++;
 
    if (q->front == NULL) {
        q->front = (Node*) malloc(sizeof(struct nodeQ));
        q->front->value = value;
        q->front->next = NULL;
        q->last = q->front;
    }

    else {
        q->last->next = (Node*) malloc(sizeof(struct nodeQ));
        q->last->next->value = value;
        q->last->next->next = NULL;
        q->last = q->last->next;
    }
}


/*-----------------------------------------------*/ 





/*----------------EDMONDS KARP--------------------*/

void avaliar(Graph graph, int posS, int posT, int* capacities) {

    if (capacities[posS] > 0 && capacities[posT] > 0) {

        if (capacities[posS] > capacities[posT]) { /* valor que vai de s para vertice > valor que vai de vertice para t */
            capacities[posS] -= capacities[posT]; /* decrementamos a ligacao de maior peso */
            contP += capacities[posT]; /* a ligacao de menor peso, como vai já ser totalmente preenchida, vai ser contabilizada no peso do corte minimo */
            capacities[posT] = 0;
            GRAPHinsertS(graph, capacities[posS], posS + 1, posS); /* inserimos apenas a ligacao cuja capacidade ainda nao esta totalmente preenchida */
        }

        else { /* situacao igual a interior mas no caso em que o maior peso e' o da ligacao do vertice para t */
            capacities[posT] -= capacities[posS];
            contP += capacities[posS];
            capacities[posS] = 0;
            GRAPHinsertT(graph, capacities[posT], posS + 1, posT);
        }
    }


    else if (capacities[posS] > 0) /* insere apenas a ligacao de s para o vertice pois a do vertice para t tem peso nulo */
        GRAPHinsertS(graph, capacities[posS], posS + 1, posS);

    else
        GRAPHinsertT(graph, capacities[posT], posS + 1, posT); /* insere apenas a ligacao do vertice par t pois a de s para o vertice peso nulo */

}





int bfs(Graph graph, int s, int t, int* capacities, bool visited[])
{

    int i;
    for(i=0; i<graph->totalV; i++){
        visited[i] = false;
    }
 
    
    visited[s] = true;
    
    Queue q;
    init(&q);
    push(&q, s);

    

    while (!(q.size == 0))
    {
    
        int u = q.front->value;
        pop(&q); /* o elemento que vai sair da fila e' o que la foi colocado ha' mais tempo (first in, first out) */
        
        
        link n;
        for (n= graph->v[u]->adj; n!=NULL; n=n->next)
        {

            if (visited[n->value] == false && capacities[n->lig] > 0) /* apenas se a capacidade (residual) for superior a zero e' que 
            															o vertice podera ser visitado */
            {   
                
                visited[n->value] = true;
                graph->v[n->value]->parent = u;
                graph->v[n->value]->lig = n->lig;
                graph->v[n->value]->bLig = n->bLig;
                push(&q, n->value); /* colocamos o elemento que acabamos de visitar na fila */
            }
        }
        
    }
 

    return (visited[t] == true); /* se t foi visitado, ha' caminho de aumento */
}





void edmondsKarp(Graph graph, int s, int t, int* capacities)
{
    
    int v;

    bool* visited = (bool*)malloc(sizeof(bool)*graph->totalV);

    while (bfs(graph, s, t, capacities, visited)) /* bfs calcula o caminho de incremento mais curto */
    {   
        
    	
        int path_flow = INT_MAX;                                                        
        
        for (v=t; v!=s; v = graph->v[v]->parent)
        {
        	
            path_flow = min(path_flow, capacities[graph->v[v]->lig]); /* calcular o fluxo que vai passar no caminho de aumento; 
            								vai ter que ser a menor capacidade residual das ligacoes do caminho de aumento*/
        }

 		
  
        for (v=t; v != s; v = graph->v[v]->parent) /* atualizacao das capacidades residuais de acordo com o path_flow calculado */
        {   

            capacities[graph->v[v]->lig] -= path_flow;
            if(graph->v[v]->bLig != -1) { /* se bLig for igual a -1 estamos num vertice sem ligacao bidirecional (neste caso so' pode ser t) */

                capacities[graph->v[v]->bLig] +=path_flow;
            }
  
        }

    }
 


    int i, j;

    for (i = 0; i < graph->totalV; i++){
        link n;
        for(n=graph->v[i]->adj; n!=NULL; n= n->next) {
            if(visited[i] && !visited[n->value] && n->cap) { /* se um vertice foi visitado mas o seu adjacente nao, o corte e' aplicado entre
            													estes dois vertices em questao */
                contP+= n->cap;
            }
        }

        if (i < graph->numV) {

            if (visited[i+1]== true) { /* i+1 porque comeca'mos com i=0 mas nao queremos avaliar a fonte */
                graph->v[i]->pixel = C;
            }

            else {
                graph->v[i]->pixel = P;
            }
        }


    }



 	printf("%d\n",contP);
 	printf("\n");
    
    int a=0;

 	for(i =0; i< graph->mLinhas; i++) {
 		for(j=0; j<graph->nColunas; j++) {

            if(graph->v[a++]->pixel == C)
                printf("C ");

            else{
                printf("P ");
            
 			}
 		
 		}

 		printf("\n");
 	}
}




/*---------------------------------------------------*/

int main(int argc, char const *argv[])
{

    int mLinhas, nColunas;
	scanf("%d %d", &mLinhas, &nColunas);
	int V = (mLinhas* nColunas) + 2; /* nº total de vertices = nº total de pixeis + vertice fonte + vertice destino */


    Graph graph = GRAPHinit(V, mLinhas, nColunas); 

    int ligacoes = (mLinhas*nColunas)*2 + (mLinhas*(nColunas-1) + nColunas*(mLinhas-1))*2; /* inclui as ligacoes inversas */

    int* capacities = (int*)malloc(sizeof(int)*ligacoes);


	getchar();

    int contLig = 0; /* funciona como identificador da ligacao */
	int i=1;
    int u,v,value; /* value = peso/capacidade da ligacao */
	for (u=0; u<mLinhas; u++) {
		for (v=0; v<nColunas; v++) {
			scanf("%d", &value);
            capacities[contLig++] = value; /* posicao em que "value" e' colocado no vetor de capacidades = valor da lig no vertice */
			getchar();
			i++;

		}

	}


	getchar();

	i=1;
	for (u=0; u<mLinhas; u++) {
		for (v=0; v<nColunas; v++) {
			scanf("%d", &value);
            capacities[contLig] = value;

            if ((!(capacities[i-1] == 0)) || (!(capacities[contLig] == 0))) { /* se tanto o peso que vai de s para o vertice, como do vertice para o t for 
                                                                                nulo, as ligacoes nao sao inseridas no grafo */
                avaliar(graph, i-1, contLig, capacities); /* i-1 = posicao (no vetor capacities) em que o peso da ligacao de s para um determinado vertice 
                                                    esta guardado; contLig = posicao em que o peso da ligacao desse mesmo vertice para t esta guardado */
            }
			contLig++;
            getchar();
			i++;

		}

	}



	if (nColunas != 1) {
		getchar(); /* so e' necessario fazer getchar se houver mais do que uma coluna pois se apenas houver uma, 
					o proximo ciclo nao sera executado */
	}

	int cont=1;
	
    for (u=0; u<mLinhas; u++) {
		for (v=0; v<nColunas-1; v++) {

			scanf("%d", &value); 
            GRAPHinsertHorizontalE(graph,value, cont, contLig);
            capacities[contLig++] = value;
            capacities[contLig++] = value; /* ocupamos duas posicoes com o mesmo value porque estamos a considerar a ligacao contraria */

			cont++;

			getchar();

		}
		cont++;

	}



	if (mLinhas != 1) { /* so e' necessario fazer getchar se houver mais do que uma linha pois se apenas houver uma, 
						o proximo ciclo nao sera executado */
		getchar();
	}


	cont=1;

	for (u=0; u < mLinhas-1; u++) {
		for (v=0; v < nColunas; v++) {

			scanf("%d", &value); 
            GRAPHinsertVerticalE(graph, value, cont, contLig);
            capacities[contLig++] = value;
            capacities[contLig++] = value; /* ocupamos duas posicoes com o mesmo value porque estamos a considerar a ligacao contraria */

			getchar();
			cont++;

		}

	}


    edmondsKarp(graph, 0, V-1,capacities);


	return 0;
}
