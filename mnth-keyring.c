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
#define GET_KEY(ptr) ((mnth_keys*)ptr)

static struct _global_keyring {
  dlist_t keyring;
  pthread_mutex_t lock;
} global_keyring = {
  .keyring = DLIST_HEAD_INIT(G.keyring),
  .lock = PTHREAD_MUTEX_INITIALIZER
};

#define keyring_lock pthread_mutex_lock
#define keyring_unlock pthread_mutex_unlock

static void *
key_alloc0(void)
{
  mnth_keys *new_mem = malloc(sizeof(mnth_keys));
  if (!new_mem)
    return NULL;
  memset(new_mem, 0, sizeof(*new_mem));
  return new_mem;
}

char *
mnth_keyring_add(const char *key, size_t keylen)
{
  mnth_keys *new_key;

  if (!key)
    return NULL;

  if (mnth_keyring_lookup(key))
    return NULL;

  new_key = key_alloc0();
  if (!new_key)
    return NULL;

  if (keylen > sizeof(new_key->key))
    keylen = sizeof(new_key->key);
  memcpy(new_key->key, key, keylen);
  new_key->keylen = keylen;

  keyring_lock(&G.lock);

  /* TODO
   * it should be hashmap instead of linked-list,
   * but using until examine 'topkeys' properly */
  dlist_append(&G.keyring, GET_DLIST(new_key));

  keyring_unlock(&G.lock);

  return new_key;
}

int
mnth_keyring_lookup(const char *key)
{
  int ret = 0;

  if (!key)
    return ret;

  keyring_lock(&G.lock);

  dlist_foreach(&G.keyring) {
    mnth_keys *item = GET_KEY(__ptr);
    if (!memcmp(key, item->key, item->keylen)) {
      ret = 1;
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

void
mnth_keyring_iter(void (*cb)(mnth_keys *key))
{
  if (!cb)
    return;

  dlist_foreach_safe(&G.keyring)
    cb(GET_KEY(__ptr));
}

/**
 * mini quick dump
 */
void
mnth_keyring_dump(void)
{
  char buf[KEYLEN] = { 0, };

  dlist_foreach_safe(&G.keyring) {
    mnth_keys *key = GET_KEY(__ptr);
    memcpy(buf, key->key, KEYLEN);
    buf[key->keylen] = 0;
    fprintf(stderr, "[%s:%d] MNTH - key (%s)\n", __func__, __LINE__, buf);
  }
}
