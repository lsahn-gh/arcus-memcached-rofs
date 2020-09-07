/**
 * mnth-event-thread.c
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

#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#include "mnth-helper.h"

#define G evt_buffer

static struct _evt_buffer {
  dlist_t events;
  pthread_mutex_t lock;
} evt_buffer = {
  .events = DLIST_HEAD_INIT(G.events),
  .lock = PTHREAD_MUTEX_INITIALIZER
};

#define buf_lock pthread_mutex_lock
#define buf_unlock pthread_mutex_unlock

/* called at only engine side */
int
mnth_evtbuf_enqueue_async(dlist_t *item)
{
  if (!item)
    return 0;

  buf_lock(&G.lock);
  dlist_enqueue(&G.events, item);
  buf_unlock(&G.lock);

  return 1;
}

static void
do_process_event_queue(void)
{
  mnth_evt_item *evt = NULL;
  mnth_keys *obj = NULL;
  uint32_t flag = 0;
  size_t ratelimit = 0;

        /* ratelimit 10 times per poll */
  for (; ratelimit < 10; ratelimit++)
  {
    buf_lock(&G.lock);
    if (dlist_empty(&G.events)) {
      buf_unlock(&G.lock);
      break;
    }
    evt = GET_EVT(dlist_dequeue(&G.events));
    buf_unlock(&G.lock);

    flag = evt->flag;
    obj = GET_KEY(evt->obj);

    free(evt);

    switch(flag) {
    case MNTH_EVT_KEYADD:
      {
        mnth_keys *rkey = GET_KEY(mnth_key_cache_lookup(obj->key));
        if (!rkey)
          mnth_key_cache_add(obj);
        else {
          free(obj);
          obj = NULL;
        };
      }
      break;
    case MNTH_EVT_KEYDEL:
      {
        mnth_keys *rkey = GET_KEY(mnth_key_cache_lookup(obj->key));
        if (rkey) {
          mnth_key_cache_rm(rkey);
          free(rkey);
        }
      }
      break;
    default:
      /* Nothing to do */;
    }

    /* be careful! */
    if (obj && !(flag & MNTH_EVT_KEYADD))
      free(obj);
  }
}

static void *
run_evtbuf_poll(void *arg)
{
  for (;;)
  {
    do_process_event_queue();

    usleep(500*1000);
  }

  return NULL;
}

pthread_t
mnth_event_thread_new(void *arg)
{
  pthread_t id;

  if (pthread_create(&id, NULL, run_evtbuf_poll, arg) != 0)
    return -errno;

  return id;
}
