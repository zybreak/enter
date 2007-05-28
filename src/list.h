#ifndef __LIST_H__
#define __LIST_H__

/**
 * This is a linked list.
 * The head is empty, so as not to require
 * passing pointers to pointers all the time.
 */
typedef struct list_t {
	void *data;
	struct list_t *next;
} list_t;

/**
 * Create a new linked list.
 * @return A new list_t object.
 */
list_t* list_new(void);

/**
 * Delete a list created by list_new.
 * @param list The list to free.
 */
void list_delete(list_t *list);

/**
 * Add a new element to a list.
 * @param list The list to add data to.
 * @param data The data to add to list.
 */
void list_add(list_t *list, void *data);

/**
 * Fetch the next element from a list.
 * @param list The list to fetch a element from.
 * @return The next position in the list.
 * @return NULL if the end is reached.
 */
list_t* list_next(list_t *list);

/**
 * Get the data of a element.
 * @param list The element to fetch data from.
 * @return The data from element.
 */
void* list_data(list_t *list);

#endif
