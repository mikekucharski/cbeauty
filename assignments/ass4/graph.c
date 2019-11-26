#include <stdio.h>
#include <stdlib.h>

typedef struct Vertex {
  int pid;
  int visited;
  int waiting_for_spot;
  struct Vertex* waiting_for;
  // struct Vertex *next;
} Vertex;

typedef struct Graph {
  struct Vertex* vertices;
  int size;
} Graph;

Graph* initializeGraph(int count) {
  Graph* g = (Graph*)malloc(sizeof(Graph));
  g->size = count;
  // Create an array of vertexes. Size of array is count
  g->vertices = (struct Vertex*)malloc(count * sizeof(struct Vertex));

  // Initialize each adjacency list as empty by making head as NULL
  int i;
  for (i = 0; i < count; ++i) {
    g->vertices[i].pid = i;
    g->vertices[i].visited = 1;
    g->vertices[i].waiting_for_spot = -1;
    g->vertices[i].waiting_for = NULL;
  }
  return g;
}

void printGraph(Graph* graph) {
  printf("============================\n");
  int i;
  for (i = 0; i < graph->size; i++) {
    Vertex v = graph->vertices[i];
    int waiting_for_pid;
    if (v.waiting_for == NULL) {
      waiting_for_pid = -1;
    } else {
      waiting_for_pid = v.waiting_for->pid;
    }
    printf(
        "Vertex: %d, pid: %d, Visited: %d, Waiting_For_Spot: %d, Waiting_for "
        "pid: %d\n",
        i, v.pid, v.visited, v.waiting_for_spot, waiting_for_pid);
  }
  printf("============================\n");
}

int waitFor(Graph* graph, int pid, int waiting_on_id, int resourceID) {
  if (pid >= graph->size || waiting_on_id >= graph->size) {
    return -1;
  }
  graph->vertices[pid].waiting_for = &graph->vertices[waiting_on_id];
  graph->vertices[pid].waiting_for_spot = resourceID;
  return 1;
}

int removeWait(Graph* graph, int pid) {
  if (pid >= graph->size) {
    return -1;
  }
  graph->vertices[pid].waiting_for = NULL;
  graph->vertices[pid].waiting_for_spot = -1;
  return 1;
}

void unvisitAllNodes(Graph* graph) {
  int i;
  for (i = 0; i < graph->size; i++) {
    graph->vertices[i].visited = 0;
  }
}

int processWaitingFor(Graph* graph, int resourceID) {
  int i;
  for (i = 0; i < graph->size; i++) {
    if (graph->vertices[i].waiting_for_spot == resourceID) {
      return graph->vertices[i].pid;
    }
  }
  return -1;
}

int checkForCycle(Graph* graph) {
  int i;
  for (i = 0; i < graph->size; i++) {
    unvisitAllNodes(graph);
    Vertex* v = &graph->vertices[i];
    v->visited = 1;
    v = v->waiting_for;
    while (v != NULL) {
      if (v->visited == 1) {
        return 1;
      } else {
        v->visited = 1;
        v = v->waiting_for;
      }
    }
  }
  unvisitAllNodes(graph);
  return 0;
}

int main(int argc, char* argv[]) {
  Graph* graph = initializeGraph(5);
  int i;
  for (i = 0; i < graph->size; i++) {
    printf("ID: %d\n", graph->vertices[i].pid);
  }

  waitFor(graph, 0, 1, 0);
  waitFor(graph, 1, 2, 1);
  waitFor(graph, 2, 3, 2);
  waitFor(graph, 3, 4, 3);

  int cycleFound;
  cycleFound = checkForCycle(graph);
  printf("Cycle is %d", cycleFound);

  printGraph(graph);
  unvisitAllNodes(graph);
  printGraph(graph);

  int process = processWaitingFor(graph, 3);
  printf("PID is %d", process);

  removeWait(graph, 3);
  printGraph(graph);
}