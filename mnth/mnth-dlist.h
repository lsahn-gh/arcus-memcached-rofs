/**
 * mnth-dlist.h
 *
 * backport dlist.h from https://github.com/memnoth/portable-cds
 */

#ifndef _MNTH_DLIST_H_
#define _MNTH_DLIST_H_

#include <stdlib.h>
#include <stdint.h>

#if 1
/* Use ARCUS's dlist instead of backported type. */
#include "topkeys.h"
#else
typedef struct dlist dlist_t;
struct dlist {
  struct dlist *prev, *next;
};
#endif

#define GET_DLIST(ptr)  ((dlist_t *)ptr)

#define DLIST_HEAD_INIT(name) { &(name), &(name) }
#define DLIST_HEAD(name) dlist_t name = DLIST_HEAD_INIT(name)

static inline void dlist_init_head(dlist_t *__head)
{
  if (__head == NULL)
    return;

  __head->next = __head;
  __head->prev = __head;
}

static inline int __list_add(dlist_t *__new,
                             dlist_t *prev,
                             dlist_t *next)
{
  if (__new == NULL)
    return -1;

  __new->next = next;
  __new->prev = prev;
  prev->next = __new;
  next->prev = __new;

  return 0;
}

static inline int dlist_append(dlist_t *__head, dlist_t *item)
{
  if (__head == NULL || item == NULL)
    return -1;

  return __list_add(item, __head->prev, __head);
}

static inline int __list_del(dlist_t *entry)
{
  if (entry == NULL)
    return -1;

  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;

  return 0;
}

static inline int dlist_remove(dlist_t *item)
{
  if (item == NULL)
    return -1;

  return __list_del(item);
}

static inline int dlist_empty(dlist_t *head)
{
  if (head == NULL)
    return 1;

  return head->next == head;
}

#define dlist_foreach(__head) \
  for (dlist_t *__ptr = (__head)->next; __ptr != (__head); __ptr = __ptr->next)

#define dlist_foreach_safe(__head) \
  for (dlist_t *__ptr, *__n = (__ptr = (__head)->next, __ptr->next); __ptr != (__head); __ptr = __n, __n = __ptr->next)

#endif
