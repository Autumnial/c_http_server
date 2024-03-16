// double linked list

#ifndef LinkedList_H
#define LinkedList_H

typedef struct _Node {
    void         *data;
    struct _Node *next;
    struct _Node *prev;
} _ll_Node;

typedef struct _LinkedList {
    _ll_Node *_head;
    _ll_Node *_tail;
    int       length;
    int       (*compare)(void *a, void *b);
} LinkedList;

// basics first, might expand later
// TODO: add functional stuff like map, filter, etc
LinkedList *ll_new_linked_list(int (*compare)(void *a, void *b));
void        ll_push(LinkedList *list, void *data);
void       *ll_get(LinkedList *list, int index);
void       *ll_remove(LinkedList *list, int index);
int         ll_contains(LinkedList *list, void *data);
LinkedList *ll_filter(LinkedList *list, int (*filter)(void *data));
void        ll_insert(LinkedList *list, int index, void *data);
void        ll_destroy(LinkedList *list);
void       *ll_get_by_value(LinkedList *list, void *value);

#endif // !LinkedList_H
