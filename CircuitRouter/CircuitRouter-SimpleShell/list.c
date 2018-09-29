#include "list.h"
#include "Process.h"
#include <stdlib.h> 
#include <stdio.h>

Node createNode(Process * p) {
    Node n = malloc(sizeof(struct node));
    n->item = p;
    n->next = NULL;
    
    return n;
}

void freeNode(Node p) {
    freeProcess(p->item);
    free(p);
}

void insert(Node head, Node new) {
    new->next = head->next;
    head->next = new;
}

void removeByPID(int pid, Node head) {
    for (Node h = head; h != NULL && h->next != NULL; h = h->next)
        if (h->next != NULL && getPid(h->next->item) == pid) {
            Node aux = h->next;
            h->next = h->next->next;
            free(aux);
            return;
        }
}

Process *getByPID(int pid, Node head) {
    for (Node h = head->next; h != NULL; h = h->next)
        if (getPid(h->item) == pid) 
            return h->item;
    return NULL;
}

void freeAll(Node head) {
    for (Node h = head, aux = h; h != NULL; aux = h) {
        h = h->next;
        freeNode(aux);
    }
}

Node next(Node h) {
    return h->next;
}

void printAll(Node h) {
    for (Node a = h->next; a != NULL; a = a->next) 
        printProcess(a->item);
}