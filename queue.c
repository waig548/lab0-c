#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Queue structure */
typedef struct {
    // The head of the queue
    struct list_head head;
    // The size of the queue
    int size;
} queue_t;

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    // allocate space for the queue
    queue_t *q = (queue_t *) malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    // initialize the queue
    LIST_HEAD(tmp);
    q->head = tmp;
    INIT_LIST_HEAD(&q->head);
    q->size = 0;

    return &q->head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // if l is NULL, do nothing
    if (!l)
        return;
    // traverse the queue and free the elements
    struct list_head *pos, *q;
    list_for_each_safe (pos, q, l) {
        element_t *e = list_entry(pos, element_t, list);
        // remove the node from the queue
        list_del(pos);
        // free the element
        q_release_element(e);
    }
    // get and release the queue container
    queue_t *qu = container_of(l, queue_t, head);
    free(qu);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    // if head is NULL, return false
    if (!head)
        return false;

    // allocate space for the element
    element_t *e = (element_t *) malloc(sizeof(element_t));
    if (!e)
        return false;

    // make a node for the element
    LIST_HEAD(tmp);
    e->list = tmp;

    // allocate space for the string
    e->value = (char *) malloc(strlen(s) + 1);
    if (!e->value) {
        free(e);
        return false;
    }
    // copy the string into the element
    memcpy(e->value, s, strlen(s) + 1);

    // insert the node at the head of the queue
    list_add(&e->list, head);

    // increment the size of the queue
    container_of(head, queue_t, head)->size++;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    // if head is NULL, return false
    if (!head)
        return false;

    // allocate space for the element
    element_t *e = (element_t *) malloc(sizeof(element_t));
    if (!e)
        return false;

    // make a node for the element
    LIST_HEAD(tmp);
    e->list = tmp;

    // allocate space for the string
    e->value = (char *) malloc(strlen(s) + 1);
    if (!e->value) {
        free(e);
        return false;
    }
    // copy the string into the element
    memcpy(e->value, s, strlen(s) + 1);

    // insert the node at the tail of the queue
    list_add_tail(&e->list, head);

    // increment the size of the queue
    container_of(head, queue_t, head)->size++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // if head is NULL or empty, return NULL
    if (!head || list_empty(head))
        return NULL;

    // get the element at the head of the queue
    element_t *e = list_entry(head->next, element_t, list);
    // remove the element from the queue
    list_del(&e->list);
    // decrement the size of the queue
    container_of(head, queue_t, head)->size--;

    // if sp is not NULL, copy the string into it
    if (sp) {
        memcpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // if head is NULL or empty, return NULL
    if (!head || list_empty(head))
        return NULL;

    // get the element at the tail of the queue
    element_t *e = list_entry(head->prev, element_t, list);
    // remove the element from the queue
    list_del(&e->list);
    // decrement the size of the queue
    container_of(head, queue_t, head)->size--;

    // if sp is not NULL, copy the string into it
    if (sp) {
        memcpy(sp, e->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return e;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    // if head is NULL or empty, return 0
    if (!head || list_empty(head))
        return 0;
    /*
    int c=0;
    struct list_head *pos;
    list_for_each(pos, head) {
        c++;
    }
    return c;
    */
    return container_of(head, queue_t, head)->size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return NULL if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    // if head is NULL or empty, return NULL
    if (!head || list_empty(head))
        return false;

    // get the middle node
    int i = 0, tgt = (q_size(head) + 1) / 2 - 1;
    struct list_head *pos;
    list_for_each (pos, head) {
        if (i == tgt) {
            element_t *e = list_entry(pos, element_t, list);
            list_del(&e->list);
            // delete the element
            q_release_element(e);
            container_of(head, queue_t, head)->size--;
            return true;
        }
        i++;
    }
    return false;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    // if head is NULL, return false
    if (!head)
        return false;

    LIST_HEAD(dump_yard);
    // dummy node to mark the end of the queue
    q_insert_tail(head, "");
    // traverse the list and determine duplicate groups
    struct list_head *pos = head->next, *ref = NULL, *end = NULL;
    while (pos != head) {
        // if current string is same as the reference,
        // extend the group range
        if (ref && !strcmp(list_entry(ref, element_t, list)->value,
                           list_entry(pos, element_t, list)->value))
            end = pos;
        else {
            // if group size > 1, i.e. ref != end,
            // move the group to the dump yard
            if (ref && end && ref != end) {
                // a workaround so that the dump yard doesn't get
                // overriden by the next group
                LIST_HEAD(group);
                list_cut_position(&group, ref->prev, end);
                list_splice(&group, &dump_yard);
            }
            ref = pos;
            end = NULL;
        }
        pos = pos->next;
    }
    // empty the dump yard
    list_for_each_safe (pos, end, &dump_yard) {
        element_t *e = list_entry(pos, element_t, list);
        list_del(&e->list);
        // delete the element
        q_release_element(e);
        container_of(head, queue_t, head)->size--;
    }
    // remove the dummy node
    q_release_element(q_remove_tail(head, NULL, 0));
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    // if head is NULL or empty or singular, return
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    int odd = q_size(head) % 2;
    LIST_HEAD(tmp);
    if (odd)
        list_add_tail(&tmp, head);
    struct list_head *cur = head->next;
    while (cur != head) {
        list_move(cur->next, cur->prev);
        cur = cur->next;
    }
    if (odd)
        list_del(&tmp);
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head) {}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head) {}
