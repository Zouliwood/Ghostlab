#ifndef LIST
#define LIST
#include <stdint.h>
#include <pthread.h>

struct element
{
    void*              data;
    struct element*  next;
    struct element*  prev;
};
typedef struct element element;

struct listElements
{
    struct element*   first;
    struct element*   last;
    uint8_t count;
};
typedef struct listElements listElements;


//return 0 if success, -1 if error
int                 removeEl(struct listElements *list, struct element *el);
struct element*   addEl(struct listElements *list, struct element *prevEl, void *data);

#endif
