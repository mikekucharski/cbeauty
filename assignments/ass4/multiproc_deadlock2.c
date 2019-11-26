/**********************************************
 *  IPC One parent process NPROC children     *
 **********************************************/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define NPROC (16) /* number of children processes */
#define TRUE 1
#define FALSE 0
#define MARKED 1    /* lock reserved (lock is taken by a child) */
#define NOTMARKED 0 /* lock available (no child process owns this lock) */
#define MAXCHILDLOCKS                                                       \
  2 /* max resource a child can hold before requesting 'release locks' from \
     the LockManager */

#define NO_DEADLOCK 0       /* there is no deadlock */
#define DEADLOCK_DETECTED 1 /* Deadlock detected   */
#define MAXLOCKS 10 /* Total available resources (size of the lock table) */

/*
 * Children send this message to request a lock.
 */
#define LOCK 100    /* Child requests to lock a resource */
#define RELEASE 200 /* Child requests to release all its resources */
struct msg_requestLock {
  int lockID; /* this a number from 0 up to (MAXLOCKS-1) */
  int Action; /* LOCK, RELEASE */
};

/*
 * LockManager sends status of request to children
 */
#define GRANTED 0
#define NOT_GRANTED 1
#define YOU_OWN_IT 2
#define PREVENT 3
struct msg_LockStatus {
  int status;
  int by_child; /* if not granded, who owns it */
};

/*
 * Structure the LockManager holds (this is a single lock)
 */
struct lock {
  int marked;
  int by_child;
};

/*
 * 'lock' holds all the resources
 */
struct lock locks[MAXLOCKS]; /* MAXLOCKS locks for the manager */

int deadlock = NO_DEADLOCK; /* When deadlock occurs, exit    */
int pid[NPROC];             /* Process ids              */

// #################################################
// Start ===================================== Start
// #################################################

typedef struct Vertex {
  int pid;
  int visited;
  int waiting_for_spot;
  struct Vertex *waiting_for;
  // struct Vertex *next;
} Vertex;

typedef struct Graph {
  struct Vertex *vertices;
  int size;
} Graph;

Graph *initializeGraph(int count) {
  Graph *g = (Graph *)malloc(sizeof(Graph));
  g->size = count;
  // Create an array of vertexes. Size of array is count
  g->vertices = (struct Vertex *)malloc(count * sizeof(struct Vertex));

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

void printGraph(Graph *graph) {
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

int waitFor(Graph *graph, int pid, int waiting_on_id, int resourceID) {
  if (pid >= graph->size || waiting_on_id >= graph->size) {
    return -1;
  }
  graph->vertices[pid].waiting_for = &graph->vertices[waiting_on_id];
  graph->vertices[pid].waiting_for_spot = resourceID;
  return 1;
}

int removeWait(Graph *graph, int pid) {
  if (pid >= graph->size) {
    return -1;
  }
  graph->vertices[pid].waiting_for = NULL;
  graph->vertices[pid].waiting_for_spot = -1;
  return 1;
}

void unvisitAllNodes(Graph *graph) {
  int i;
  for (i = 0; i < graph->size; i++) {
    graph->vertices[i].visited = 0;
  }
}

int processWaitingFor(Graph *graph, int resourceID) {
  int i;
  for (i = 0; i < graph->size; i++) {
    if (graph->vertices[i].waiting_for_spot == resourceID) {
      return graph->vertices[i].pid;
    }
  }
  return -1;
}

int checkForCycle(Graph *graph) {
  int i;
  for (i = 0; i < graph->size; i++) {
    unvisitAllNodes(graph);
    Vertex *v = &graph->vertices[i];
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

// #############################################
// END ===================================== END
// #############################################

Graph *graph;

/*
 * Called at the end to cleanup
 */
void finish() {
  int i;
  for (i = 0; i < NPROC; i++) kill(pid[i], 9);
  free(graph);
  exit(0);
}

/*
 * Code for the child processes
 */
void child(int pid, int req, int ack) {
  int rand_lock; /* a random lock request      */
  int count = 0; /* It is used for num of locks  */

  struct msg_requestLock MSG; /* message from child (me) to parent */
  struct msg_LockStatus STAT; /* message from parent to child (me) */

  // set up seed for random num generator
  struct timeval tt;
  (void)gettimeofday(&tt, NULL);
  srand(tt.tv_sec * 1000000 + tt.tv_usec);

  for (;;) {
    MSG.lockID = rand() % MAXLOCKS;
    MSG.Action = LOCK;
    printf("\tChild %d: Requesting lock %d . . .\n", pid, MSG.lockID);
    fflush(stdout);
    /*
     * Both calls are blocked if there is nothing to do.
     */

    write(req, (char *)&MSG, sizeof(MSG));
    read(ack, (char *)&STAT, sizeof(STAT));

    if (STAT.status == GRANTED) { /* Success got lock */
      count++;
      printf("\tChild %d: Got lock %d (%d total).\n", pid, MSG.lockID, count);
      fflush(stdout);
    }

    //#ifdef TRACE
    // if( STAT.status == GRANTED )
    // 	printf("\tChild %d: Got lock.\n", pid);
    // else
    if (STAT.status == NOT_GRANTED) {
      printf("\tChild %d: Child %d owns this lock.\n", pid, STAT.by_child);
    } else if (STAT.status == YOU_OWN_IT) {
      printf("\tChild %d: I own this lock.\n", pid);
    }

    printf("\tChild %d: Owns %d locks now.\n", pid, count);
    fflush(stdout);
    //#endif

    if (STAT.status == NOT_GRANTED) {
      printf("Child %d waiting for lock %d\n", pid, MSG.lockID);
      fflush(stdout);

      /*
       * I will get it shortly or the LockManager
       * will NOT give it to me to prevent a deadlock.
       */
      read(ack, (char *)&STAT, sizeof(STAT));
      if (STAT.status == GRANTED) {
        count++;
        printf("\tChild %d: Got lock %d (%d).\n", pid, MSG.lockID, count);
      } else if (STAT.status == PREVENT) {
        printf("CHILD: %d Will try again, (preventing)\n", pid);
        fflush(stdout);
      } else {
        printf("CHILD: %d   FATAL ERROR\n", pid);
        fflush(stdout);
        exit(-1);
      }
    }

    if (count >= MAXCHILDLOCKS) {
      /*
       * Child sends request to release all its locks
       */
      printf("\tChild %d: Requesting RELEASE locks.\n", pid);
      fflush(stdout);

      MSG.Action = RELEASE;
      write(req, (char *)&MSG, sizeof(struct msg_requestLock));

      count = 0;

      sleep(1);
    }

  } /* for(;;) */
} /* child */

int CheckForDeadLock() {
  if (checkForCycle(graph)) {
    return (DEADLOCK_DETECTED);
  } else {
    return (NO_DEADLOCK);
  }
}

/*******************************************************
 * LockManager():                                      *
 *  Task to determine deadlock                         *
 *  Also to release and give locks                     *
 *  Terminates when LiveLock is detected               *
 *******************************************************/
int LockManager(int q, struct msg_requestLock ChildRequest,
                int respond[NPROC]) {
  int i;
  struct msg_LockStatus STAT;
  int deadlock = NO_DEADLOCK;

  if (ChildRequest.Action == RELEASE) {
    /*
     *    ***TODO*** (1)
     * Release child's resources. Give resources to children that might be
     * waiting for these resources.
     */

    // loop through all locks
    for (i = 0; i < MAXLOCKS; i++) {
      // if you find a resources locked by current child
      if (locks[i].marked && locks[i].by_child == q) {
        // if there is anyone else waiting for this resource, take next
        // available
        int processID;
        processID = processWaitingFor(graph, i);
        if (processID == -1) {  // nobody is waiting
          locks[i].marked = NOTMARKED;
          locks[i].by_child = -1;
        } else {
          removeWait(graph, processID);

          locks[i].marked = MARKED;
          locks[i].by_child = processID;

          STAT.status = GRANTED;
          STAT.by_child = -1;
          write(respond[processID], (char *)&STAT, sizeof(STAT));
        }
      }
    }
  }

  if (ChildRequest.Action == LOCK) {
    int t_lock;
    t_lock = ChildRequest.lockID;
    if (locks[t_lock].marked == NOTMARKED) {
      /*
       *    ***TODO*** (2)
       * Give requested lock to child
       */
      locks[t_lock].marked = MARKED;
      locks[t_lock].by_child = q;

      STAT.status = GRANTED;
      STAT.by_child = -1;
      write(respond[q], (char *)&STAT, sizeof(STAT));
    } else { /* lock is not free */
      if (locks[t_lock].by_child == q) {
        /*
         * Tell child that this lock is already owned by this (the requestor)
         * child
         */
        STAT.status = YOU_OWN_IT;
        write(respond[q], (char *)&STAT, sizeof(STAT));
      } else { 
        /* lock taken by another child */
        /*
         *    ***TODO*** (3)
         * Lock is owned by another child, need to wait!
         * Set up the waiting list.
         */
        waitFor(graph, q, locks[t_lock].by_child, t_lock);

        /*
         * Now tell the child that the Lock will not be given (because it's
         * owned by someone else.
         */
        STAT.status = NOT_GRANTED;
        STAT.by_child = locks[t_lock].by_child;
        write(respond[q], (char *)&STAT, sizeof(STAT));

        /*
         *    ***TODO*** (4)
         * Print the lock table and the waiting list so that YOU see what your
         * program is doing
         */
        printf("Printing lock table:\n");
        for (i = 0; i < MAXLOCKS; i++) {
          printf("Lock %d is owned by Child %d and marked as %d\n", i,
                 locks[i].by_child, locks[i].marked);
        }
        printGraph(graph);
        fflush(stdout);

        /*
         *    ***TODO*** (5)
         * Implement the 'CheckForDeadLock()' function.  If you see a deadlock
         * return DEADLOCK_DETECTED as shown below.
         */
        if (CheckForDeadLock() == DEADLOCK_DETECTED) {
          printf("DEADLOCK DETECTED. NOW ROLLBACK\n");
          fflush(stdout);
          /*
           *    ***TODO*** (6)
           * Detected Deadlock! you need to prevent it.  Rollback so that you
           * will not give the lock to the requestor.
           */
          removeWait(graph, q);

          /*
           * OK we rolledback, now notify the child that the lock will not be
           * given to it in order to prevent the deadlock.
           */
          STAT.status = PREVENT;
          write(respond[q], (char *)&STAT, sizeof(STAT));

          /*
           *    ***TODO*** (7)
           * Now that we prevented the deadlock check to see if there are any
           * free locks.  If not, we will be preventing deadlocks for ever. If
           * there are no more free locks/resources then we got a LiveLock and
           * we should
           * 1. Print a message on the screen
           * 2. Print the lock table and the waiting list/graph.
           * 3. Terminate.
           */
          int currentLocks = 0;
          // loop through all locks
          for (i = 0; i < MAXLOCKS; i++) {
            if (locks[i].marked == MARKED) {
              currentLocks++;
            }
          }
          if (currentLocks == MAXLOCKS) {
            printf(
                "A Live Lock has been detected. All resources are being "
                "used.\n");
            printGraph(graph);
            printf("Printing lock table:\n");
            for (i = 0; i < MAXLOCKS; i++) {
              printf("Lock %d is owned by Child %d and marked as %d\n", i,
                     locks[i].by_child, locks[i].marked);
            }
            printf("Exiting program because of Live Lock.\n");
            fflush(stdout);
            finish();
            exit(0);
          }
        }
      }
    }
  }

  // if ChildRequest.Action is neither RELEASE nor LOCK, you got a protocol
  // error.
  return (deadlock);
}

/******************************************************************
 * Main
 ******************************************************************/
int main(int argc, char **argv) {
  int i;
  int listen[NPROC];
  int respond[NPROC];
  int ii;

  struct msg_requestLock ChildRequest;

  /* Arrange for termination to call the cleanup procedure */
  signal(SIGTERM, finish);

  graph = initializeGraph(NPROC);
  /* initialize, don't carre about child */
  for (ii = 0; ii < MAXLOCKS; ii++) {
    locks[ii].marked = NOTMARKED;
  }

  /* Initialize pipes and fork the children processes */
  for (i = 0; i < NPROC; i++) {
    int parent_to_child[2];
    int child_to_parent[2];

    /* Create the child -> parent pipe. */
    pipe(child_to_parent);
    listen[i] = child_to_parent[0];
    fcntl(listen[i], F_SETFL, O_NONBLOCK);

    /* Create the parent -> child pipe. */
    pipe(parent_to_child);
    respond[i] = parent_to_child[1];
    fcntl(respond[i], F_SETFL, O_NONBLOCK);

    /* Create child process. */
    if ((pid[i] = fork()) == 0) {
      /* Child process code. */
      signal(SIGTERM, SIG_DFL);
      close(parent_to_child[1]);
      close(child_to_parent[0]);

      child(i, child_to_parent[1], parent_to_child[0]);

      _exit(0);
    }

    close(child_to_parent[1]);
    close(parent_to_child[0]);
  }

  /*
   * For this assignment there will never be a deadlock because our
   * LockManager prevents deadlocks
   */
  while (deadlock != DEADLOCK_DETECTED) {
    int q;
    for (q = 0; q < NPROC; q++) {
      unsigned char buffer[2];
      int nb = read(listen[q], (char *)&ChildRequest, sizeof(ChildRequest));
      if (nb == sizeof(ChildRequest)) {
        deadlock = LockManager(q, ChildRequest, respond);
      }
    }
  }

  /*
   * Just be nice if your LockManager detects but does not
   * prevent a deadlocks, kill all children processes.
   */
  finish();
}