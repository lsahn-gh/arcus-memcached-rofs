/**
 * mnth-key-cache.c
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

#include "mnth-helper.h"

#define G global_cache

static struct _global_keyring {
  dlist_t caches;
  pthread_mutex_t lock;
} global_cache = {
  .caches = DLIST_HEAD_INIT(G.caches),
  .lock = PTHREAD_MUTEX_INITIALIZER
};

#define key_cache_lock pthread_mutex_lock
#define key_cache_unlock pthread_mutex_unlock

char *
mnth_key_cache_add(mnth_keys *new_key)
{
  if (!new_key)
    return NULL;

  key_cache_lock(&G.lock);

  /**
   * it should be hashmap instead of linked-list,
   * but using until examine 'topkeys' properly.
   *
   * > 2020.09.05
   * Since I've built buffer thread, it mightn't need.
   */
  dlist_append(&G.caches, GET_DLIST(new_key));

  key_cache_unlock(&G.lock);

  return new_key;
}

char *
mnth_key_cache_lookup(const char *key)
{
  char *ret = NULL;
  dlist_t *pos;

  if (!key)
    return ret;

  key_cache_lock(&G.lock);

  /* TODO
   * skip-list, btree, or hashmap */
  dlist_for_each(pos, &G.caches) {
    mnth_keys *item = GET_KEY(pos);
    if (!memcmp(key, item->key, item->keylen)) {
      ret = (char*)item;
      goto out;
    }
  }

out:
  key_cache_unlock(&G.lock);

  return ret;
}

/**
 * It doesn't take a responsibility for freeing
 * the removed obj from keyring list.
 *
 * @return : removed obj or NULL on failure
 * #return : (transfer ownership of the obj)
 */
mnth_keys *
mnth_key_cache_rm(mnth_keys *key)
{
  mnth_keys *ret = NULL;

  if (!key)
    return NULL;

  key_cache_lock(&G.lock);

  if (dlist_empty(&G.caches))
    goto out;
  dlist_remove(GET_DLIST(key));
  ret = key;

out:
  key_cache_unlock(&G.lock);

  return ret;
}

/**
 * Do not lock/unlock inside callback
 */
void
mnth_key_cache_iter(void (*cb)(mnth_keys *key, void*),
                  void* arg)
{
  if (!cb)
    return;

  key_cache_lock(&G.lock);

  dlist_foreach_safe(&G.caches) {
    MNTH_KEY_INCREF(GET_KEY(__ptr));
    cb(GET_KEY(__ptr), arg);
    MNTH_KEY_DECREF(GET_KEY(__ptr));
  }

  key_cache_unlock(&G.lock);
}

/**
 * mini quick dump
 */
void
mnth_key_cache_dump(void)
{
  char buf[KEYLEN];

  key_cache_lock(&G.lock);

  dlist_foreach_safe(&G.caches) {
    mnth_keys *key = GET_KEY(__ptr);
    memcpy(buf, key->key, KEYLEN);
    buf[key->keylen] = 0;
    fprintf(stderr, "[%s:%d] MNTH - key (%s)\n", __func__, __LINE__, buf);
  }

  key_cache_unlock(&G.lock);
}
