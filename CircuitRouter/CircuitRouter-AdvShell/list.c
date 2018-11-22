#include "list.h"
#include "Process.h"
#include <stdlib.h> 
#include <stdio.h>


/*******************************************************************************
 * createNode
 *******************************************************************************
*/
Node createNode(Process * p) {
    Node n = malloc(sizeof(struct node));
    n->item = p;
    n->next = NULL;
    
    return n;
}

/*******************************************************************************
 * freeNode
 *******************************************************************************
*/
void freeNode(Node p) {
    freeProcess(p->item);
    free(p);
}

/*******************************************************************************
 * insert
 *******************************************************************************
*/
void insert(Node head, Node new) {
    new->next = head->next;
    head->next = new;
}


/*******************************************************************************
 * removeByPID
 *******************************************************************************
*/
void removeByPID(int pid, Node head) {
    for (Node h = head; h != NULL && h->next != NULL; h = h->next)
        if (h->next != NULL && getPid(h->next->item) == pid) {
            Node aux = h->next;
            h->next = h->next->next;
            free(aux);
            return;
        }
}

/*******************************************************************************
 * getByPID
 *******************************************************************************
*/
Process *getByPID(int pid, Node head) {
    for (Node h = head->next; h != NULL; h = h->next)
        if (getPid(h->item) == pid) 
            return h->item;
    return NULL;
}

/*******************************************************************************
 * freeAll
 *******************************************************************************
*/
void freeAll(Node head) {
    for (Node h = head, aux = h; h != NULL; aux = h) {
        h = h->next;
        freeNode(aux);
    }
}

/*******************************************************************************
 * next
 *******************************************************************************
*/
Node next(Node h) {
    return h->next;
}

/*******************************************************************************
 * printAll
 *******************************************************************************
*/
void printAll(Node h) {
    for (Node a = h->next; a != NULL; a = a->next) 
        printProcess(a->item);
    
    printf("END.\n");
}