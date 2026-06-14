/**
 * Intrusive Doubly Circular Linked List
 *
 * Constraints:
 * 1. Strictly circular: The list never contains NULL pointers.
 * 2. Dummy head: The list must be anchored by a data-less sentinel node.
 */

#ifndef _INCLUDE_KERNEL_LIST_H
#define _INCLUDE_KERNEL_LIST_H

#include "def.h"

/**
 * Basic doubly linked list structure.
 * It is embedded directly into the data structures that need to be linked.
 */
struct list_head {
    struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

#define LIST_HEAD(name) struct list_head name = {&(name), &(name)}

/**
 * Cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct, which is not the variable
 *          name.
 */
/* The first line acts as a type safety check using GCC's typeof extension */
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })

#define list_entry(ptr, type, member) container_of(ptr, type, member)

/**
 * list_for_each_entry - iterate over a list of given type
 * @pos:    the type * to use as a loop cursor (points to the container struct).
 * @head:   the head of your circular linked list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)                 \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head);                               \
         pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each - iterate over a list
 * @pos:    the &struct list_head to use as a loop cursor.
 * @head:   the head of your circular linked list.
 *
 * This macro only iterates over the underlying list_head pointers.
 * If you need to access the actual data structure, you must manually
 * call list_entry() inside the loop block.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

static inline void __list_add(struct list_head *new_node,
                              struct list_head *prev, struct list_head *next) {
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

/* Add a new entry to the head of the list */
static inline void list_add(struct list_head *new_node,
                            struct list_head *head) {
    __list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node,
                                 struct list_head *head) {
    __list_add(new_node, head->prev, head);
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);

    entry->next = entry;
    entry->prev = entry;
}

static inline bool list_empty(struct list_head *head) {
    return (head->next == head);
}

#endif /* _INCLUDE_KERNEL_LIST_H */
