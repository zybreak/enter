#ifndef __LIST_H__
#define __LIST_H__

typedef struct list_t list_t;
struct list_t {
	void *data;
	struct list_t *next;
};

list_t* list_new(void);
void list_delete(list_t *list);
void list_add(list_t *list, void *data);
list_t* list_next(list_t *list);
void* list_data(list_t *list);

#endif
