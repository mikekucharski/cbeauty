#include<stdio.h> // printf
#include<stdlib.h> // malloc / sizeof

// A variable declaration with structure declaration. 
struct Point { 
  int x, y; 
} p1;  // The variable p1 is declared with type 'Point'.

// Processed by compiler. Creates type alias for type "struct Point*".
typedef struct Point* MikePointer;

// Processed by Pre-processor. Creates aliases for VALUES (not types, like typedef).
#define TRUE  1
#define FALSE 0

// Note: functions have to be before main.
void printPointer(struct Point* pointer) {
  printf("x1:%d y1:%d\n", pointer->x, pointer->y);
}

int main() {
  // Variable was declared when struct was defined.
  p1.x = 1;
  p1.y = 2;
  printf("x1:%d y1:%d\n", p1.x, p1.y);

  // Create new struct on the stack. Members are NOT automatically initilized to anything like in java. Printing them prints whatever bytes were in the "free" memory.
  struct Point p4;
  printf("x4:%d y4:%d\n", p4.x, p4.y);

  // You can also declare structs on the stack like this. 
  // 1. Note, you need to say struct before the type every time.
  // 2. Note, the order matters for initializing with {}.
  struct Point p2 = {1, 2};
  printf("x2:%d y2:%d\n", p2.x, p2.y);

  // Creates an initializes 2 Point objects on the stack. 
  // Note, the stack variable arr is actually a *pointer* to the first element of the array. You can access first element data using pointer syntax.
  struct Point arr[2];
  arr[0].x = 1;
  arr[0].y = 2;
  printf("x3:%d y3:%d\n", arr[0].x, arr[0].y);
  printf("arrX:%d arrY:%d\n", arr->x, arr->y); // This is equivalent to the line above.
  printf("p4:%d y4:%d\n", arr[1].x, arr[1].y);
  printf("arrX:%d arrY:%d\n", (arr + 1)->x, (arr + 1)->y); // This is equivalent to the line above. "arr + 1" means go to element 2, ie. shift entire Point size.

  // Create pointer on the stack and print memory address (the pointers value).
  struct Point* pointer;
  printf("address:%p\n", pointer);

  // Make pointer point to address of p1 (p1 is on stack). Print p1 data using pointer dereference.
  pointer = &p1;
  printf("x1:%d y1:%d\n", pointer->x, pointer->y);

  // Take a look at typedef above. MyPoint is just an alias for "struct Point*" because its annoying remembering those 3 pieces.
  MikePointer mikePointer = &p1;
  printf("x1:%d y1:%d\n", mikePointer->x, mikePointer->y);

  // Take a look at the function definition above. It takes in one type but I can use my alias for the same type.
  printPointer(mikePointer);

  // Allocate new Point object on the heap.
  // 1. Get the size of memory we need with sizeof(), size_t is a generic number type designed to be cross platform. The implementation of size_t can be swapped out on another system.
  // 2. make call to malloc to allocate new memory on the head. returns a "void*" which means pointer to any type.
  // 3. We know the type is really a struct Point, so cast the void* -> struct Point*.
  // 4. Pointer can be used just like pointer to stack object.
  // 5. Don't forget to free the heap memory!
  size_t sizeOfPoint = sizeof(struct Point);
  void* genericPointer = malloc(sizeOfPoint);
  struct Point* p5 = (struct Point*) genericPointer;
  p5->x = 5;
  p5->y = 6;
  printf("x5:%d y5:%d\n", p5->x, p5->y);
  free(p5);

  // Do the same as above but inline and using the type alias. Looks more like Java now!
  MikePointer p6 = (MikePointer) malloc(sizeof(struct Point));
  p6->x = 7;
  p6->y = 8;
  printf("x6:%d y6:%d\n", p6->x, p6->y);
  free(p6);

  // Dynamically allocated arrays:
  // There really are no arrays, just memory and pointers. The array syntax just says "increment the point N spaces, and dereference". 
  // ie. the following code just creates space for 3 Points and returns an address to the first one.
  // You can remove the "3 *" multiplier and this code will still work. The arrP[0] works even if there is one Point.
  MikePointer arrP = (MikePointer) malloc(3 * sizeof(struct Point));
  arrP->x = 7;
  arrP->y = 8;
  printf("arrP.x:%d arrP.y:%d\n", arrP[0].x, arrP[0].y);
  free(arrP);

  return 0;
}

