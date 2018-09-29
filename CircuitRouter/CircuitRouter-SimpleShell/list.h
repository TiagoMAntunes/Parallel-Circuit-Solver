#include "Process.h"

typedef struct node {
    Process * item;
    struct node * next;
} * Node;

Node createNode(Process * p);

void freeNode(Node p);

void insert(Node head, Node new);

void removeByPID(int pid, Node head);

Process * getByPID(int pid, Node head);

void freeAll(Node head);

Node next(Node h);

void printAll(Node h);