#include <stdio.h>
#include <stdlib.h>
#define INF 100000

struct Edge{
    int src, dst; //source and destination of the edge
    int weight; //weight/cost of the edge
};

struct Graph{
    int V; //number of vertices in the graph
    int E; //number of edges in the graph
    struct Edge *edge; //array of edges
};

void TakeEdges(struct Graph **graph){
    for(int i=0;i<(*graph)->E;++i)
        scanf("%d %d %d",&((*graph)->edge[i].src),&((*graph)->edge[i].dst),&((*graph)->edge[i].weight));
}

void printArray(int *array, int n){
    for(int i=0;i<n;++i)
        printf("%d ",array[i]);
    printf("\n");
}

void BellmanFord(struct Graph *graph, int source){
    int V = graph -> V;
    int E = graph -> E;
    int distance[V],pred[V];
    //initialising the distance from the source to the other vertices 
    for(int i =0;i<V;++i){
        distance[i] = INF;
        pred[i] = -1;
    }
    distance[source] = 0;
    pred[source] = source;
    for(int i=1;i<V;++i){
        for(int j=0;j<E;++j){
            int dst = graph->edge[j].dst;
            int src = graph->edge[j].src;
            int weight = graph->edge[j].weight;
            if(distance[dst]>distance[src]+weight){
                distance[dst]=distance[src]+weight;
                pred[dst] = src;
            }
            if(distance[src]>distance[dst]+weight){
                distance[src]=distance[dst]+weight;
                pred[src] = dst;
            }
        }
    }
    printf("Distance from source %d to rest of the edges: ",source);
    printArray(distance, V);
    printf("Predecessor points array: ");
    printArray(pred,V);
}

int main(){
    //n: number of nodes
    //m: number of links/edges
    int n,m;
    scanf("%d %d",&n,&m);
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    graph->V = n;
    graph->E = m;
    graph->edge = (struct Edge *)malloc(graph->E*sizeof(struct Edge));
    TakeEdges(&graph);
    for(int i = 0;i<graph->V;++i)
        BellmanFord(graph,i);
    //printGraph(graph);
    return 0;
}