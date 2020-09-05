/**
 * mnth-event-thread.h
 *
 * Copyright 2020 Leesoo Ahn <dev@ooseel.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MNTH_EVENT_THREAD_H_
#define _MNTH_EVENT_THREAD_H_

#include "mnth-keyring.h"

#define GET_EVT(ptr) ((mnth_evt_item*)ptr)

enum {
  MNTH_EVT_KEYADD  = (1 << 0),
  MNTH_EVT_KEYDEL  = (1 << 1),
};

typedef struct _mnth_evt_item mnth_evt_item;
struct _mnth_evt_item {
  dlist_t list;
  uint32_t flag;
  void *obj;
};

/* --- Public APIs --- */
pthread_t mnth_event_thread_new(void *arg);

int mnth_evtbuf_enqueue_async(dlist_t *item);

static inline void * mnth_alloc_evt(uint32_t flag, void *obj)
{
  mnth_evt_item *new_evt;

  if (!obj)
    return NULL;

  new_evt = GET_EVT(mem_alloc0(sizeof(mnth_evt_item)));
  if (!new_evt)
    return NULL;

  new_evt->flag |= flag;
  new_evt->obj = obj;

  return new_evt;
}

static inline void * mnth_evt_key(uint32_t evt_flag,
    const char *key, size_t keylen, size_t valsz, uint64_t flag)
{
  mnth_evt_item *evt = NULL;
  mnth_keys *kobj = mnth_alloc_key(key, keylen, valsz, flag);
  if (!kobj)
    return NULL;

  evt = mnth_alloc_evt(evt_flag, kobj);
  if (!evt)
    goto free_key;

  if (!mnth_evtbuf_enqueue_async(GET_DLIST(evt)))
    goto fail_evt;

  /* Do not free both key/evt on success,
   * it doesn't work with refcount */
  return evt;

fail_evt:
  free(evt);
free_key:
  free(kobj);

  return NULL;
}

#define mnth_evt_key_add(key, nkey, nbytes, key_flag) \
  mnth_evt_key(MNTH_EVT_KEYADD, key, nkey, nbytes, key_flag)

#define mnth_evt_key_del(key, nkey) \
  mnth_evt_key(MNTH_EVT_KEYDEL, key, nkey, 0, 0)

#endif
