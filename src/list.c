#include "list.h"

list_t* list_new(void)
{
	list_t *this = xmalloc(sizeof(*this));
	this->data = NULL;
	this->next = NULL;

	return this;
}

void list_delete(list_t *list)
{
	while (list) {
		list_t *next = list->next;
	
		free(list);

		list = next;
	}
}

void list_add(list_t *list, void *data)
{
	while (list->next) {
		list = list->next;
	}
		
	list->next = list_new();
	list->next->data = data;
}

list_t* list_next(list_t *list)
{
	return (list) ? list->next : NULL;
}

void* list_data(list_t *list)
{
	return list->data;
}

