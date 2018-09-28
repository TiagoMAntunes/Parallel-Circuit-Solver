#include "list.h"
#include "Process.h"
#include <stdlib.h> 
#include <stdio.h>

Node createNode(Process * p) {
    Node n = malloc(sizeof(struct node));
    n->item = p;
    n->next = NULL;
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
            free(h);
        }
}

Node getByPID(int pid, Node head) {
    for (Node h = head->next; h != NULL; h = h->next)
        if (getPid(h->item) == pid) 
            return h;
}

void freeAll(Node head) {
    Node aux;
    for (Node h = head; h != NULL; aux = h) {
        h = h->next;
        freeNode(aux);
    }
}

Node next(Node h) {
    return h->next;
}

void print(Node h) {
    for (Node a = h->next; a != NULL; a = a->next) {
        printf("Process with PID %d\n", a->item->pid);
    }
}