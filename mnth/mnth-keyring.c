/**
 * mnth-keyring.c
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

#define G global_keyring

static struct _global_keyring {
  dlist_t keyring;
  pthread_mutex_t lock;
} global_keyring = {
  .keyring = DLIST_HEAD_INIT(G.keyring),
  .lock = PTHREAD_MUTEX_INITIALIZER
};

#define keyring_lock pthread_mutex_lock
#define keyring_unlock pthread_mutex_unlock

char *
mnth_keyring_add(mnth_keys *new_key)
{
  if (!new_key)
    return NULL;

  keyring_lock(&G.lock);

  /**
   * it should be hashmap instead of linked-list,
   * but using until examine 'topkeys' properly.
   *
   * > 2020.09.05
   * Since I've built buffer thread, it mightn't need.
   */
  dlist_append(&G.keyring, GET_DLIST(new_key));

  keyring_unlock(&G.lock);

  return new_key;
}

char *
mnth_keyring_lookup(const char *key)
{
  char *ret = NULL;
  dlist_t *pos;

  if (!key)
    return ret;

  keyring_lock(&G.lock);

  /* TODO
   * skip-list, btree, or hashmap */
  dlist_for_each(pos, &G.keyring) {
    mnth_keys *item = GET_KEY(pos);
    if (!memcmp(key, item->key, item->keylen)) {
      ret = (char*)item;
      goto out;
    }
  }

out:
  keyring_unlock(&G.lock);

  return ret;
}

/**
 * It doesn't take a responsibility for freeing
 * the removed obj from keyring list.
 *
 * @return : removed obj or NULL on failure
 * #return : (transfer ownership of the obj)
 */
char *
mnth_keyring_rm(mnth_keys *key)
{
  char *ret = NULL;

  if (!key)
    return NULL;

  keyring_lock(&G.lock);

  if (dlist_empty(&G.keyring))
    goto out;
  dlist_remove(GET_DLIST(key));
  ret = key;

out:
  keyring_unlock(&G.lock);

  return ret;
}

/**
 * Do not lock/unlock inside callback
 */
void
mnth_keyring_iter(void (*cb)(mnth_keys *key, void*),
                  void* arg)
{
  if (!cb)
    return;

  dlist_foreach_safe(&G.keyring)
    cb(GET_KEY(__ptr), arg);
}

/**
 * mini quick dump
 */
void
mnth_keyring_dump(void)
{
  char buf[KEYLEN];

  keyring_lock(&G.lock);

  dlist_foreach_safe(&G.keyring) {
    mnth_keys *key = GET_KEY(__ptr);
    memcpy(buf, key->key, KEYLEN);
    buf[key->keylen] = 0;
    fprintf(stderr, "[%s:%d] MNTH - key (%s)\n", __func__, __LINE__, buf);
  }

  keyring_unlock(&G.lock);
}
