#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

int getIndex(struct Graph *graph, int n, int m,bool *visited,int source,int *distance,int *pred){
    int min = INF;
    int index = -1;
    for(int i=0;i<n;++i){
        if(visited[i]==false && distance[i]<min){
            min = distance[i];
            index = i;
        }
    }
    distance[index] = min;
    visited[index] = true;
    for(int i =0;i<m;++i){ 
        int index1 = graph->edge[i].src;
        int index2 = graph->edge[i].dst;
        int weight = graph->edge[i].weight;
        if(index1 == index && visited[index2]==false && (distance[index1]+weight<distance[index2])){
             distance[index2]=distance[index1]+weight; 
             pred[index2] = index;
             }else if(index2 == index && visited[index1]==false && (distance[index2]+weight<distance[index1]))
             { distance[index1]=distance[index2]+weight; 
                pred[index1] = index;
             } 
             }
    return index;
}

int Dijkstra(struct Graph *graph, int source){
    int count = 0;
    int V = graph -> V;
    int E = graph -> E;
    int distance[V],pred[V];
    bool visited[V];
    //initialising the distance from the source to the other vertices 
    for(int i =0;i<V;++i){
        distance[i] = INF;
        visited[i]= false;
        pred[i] = -1;
    }
    distance[source] = 0;
    pred[source] = source;
    count = 1;
    while(count<=V){
        int index = getIndex(graph,V,E,visited,source,distance,pred);
        ++count;
    }
    return distance[V-1];
}

void makeNewGraph(struct Graph *old_graph,struct Graph **graph,int down_node){
    int V = old_graph->V;
    int E = old_graph->E;
    ((*graph)->V) = V;
    int edgeCount = 0;
    for(int i = 0;i<E;++i){
        if(old_graph->edge[i].src == down_node || old_graph->edge[i].dst == down_node){
            continue;
        }else{
            (*graph)->edge[edgeCount].src = old_graph->edge[i].src;
            (*graph)->edge[edgeCount].dst = old_graph->edge[i].dst;
            (*graph)->edge[edgeCount].weight = old_graph->edge[i].weight;
            ++edgeCount;
        }
    } 
   ((*graph)->E) = edgeCount; 
}

int main(){
    //n: number of nodes
    //m: number of links/edges
    int n,m,down_node;
    scanf("%d %d",&n,&m);
    struct Graph *old_graph = (struct Graph *)malloc(sizeof(struct Graph));
    struct Graph *graph = (struct Graph *)malloc(sizeof(struct Graph));
    old_graph->V = n;
    old_graph->E = m;
    old_graph->edge = (struct Edge *)malloc(old_graph->E*sizeof(struct Edge));
    graph->edge = (struct Edge *)malloc(old_graph->E*sizeof(struct Edge));
    TakeEdges(&old_graph);
    scanf("%d",&down_node);
    makeNewGraph(old_graph,&graph,down_node);
    int shortest_D=Dijkstra(graph,0);
    if(shortest_D>=INF)
        shortest_D = -1;
    printf("The shortest distance between Alice and Bob is %d\n",shortest_D);
    return 0;
}