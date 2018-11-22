#include "Process.h"

typedef struct node {
    Process * item;
    struct node * next;
} * Node;

/*******************************************************************************
 * createNode
 * Input: Pointer to process
 * Output: Pointer to node of list
 *******************************************************************************
*/
Node createNode(Process * p);

/******************************************************************************* 
 * freeNode
 * Input: Pointer to node
 * Output: void
 * Frees node pointed by p
 *******************************************************************************
*/
void freeNode(Node p);

/******************************************************************************* 
 * insert
 * Input: Pointer to head of list
 *		  Poiter to node
 * Output: void
 * Inserts node pointed by new at beginning of list pointed by head
 *******************************************************************************
*/
void insert(Node head, Node new);

/******************************************************************************* 
 * removeByPID
 * Input: pid
 * 		  pointer to head of list
 * Output: void
 * Removes the node that points to the process with the id equal to pid from 
 * the list pointed by head
 *******************************************************************************
*/
void removeByPID(int pid, Node head);

/******************************************************************************* 
 * getByPID
 * Input: pid
 *		  Pointer to head of list
 * Output: Pointer to process with id equal to pid
 *******************************************************************************
*/
Process * getByPID(int pid, Node head);

/******************************************************************************* 
 * freeAll
 * Input: Pointer to head of list to free
 * Output: void
 *******************************************************************************
*/
void freeAll(Node head);

/******************************************************************************* 
 * next
 * Input: Pointer to node
 * Output: Pointer to node next to h
 *******************************************************************************
*/
Node next(Node h);

/******************************************************************************* 
 * printAll
 * Input: Pointer to head of list
 * Output: void
 * Prints all the processes that are pointed by the lists nodes
 *******************************************************************************
*/
void printAll(Node h);