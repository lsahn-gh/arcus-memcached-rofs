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
#define KEYRING keyring
#define GET_KEY(ptr) ((mnth_keys*)ptr)

static struct _global_keyring {
  dlist_t keyring;
  pthread_mutex_t lock;
} global_keyring = {
  .keyring = DLIST_HEAD_INIT(G.keyring),
  .lock = PTHREAD_MUTEX_INITIALIZER
};

static void *
key_new_alloc0(void)
{
  mnth_keys *new_mem = malloc(sizeof(mnth_keys));
  if (!new_mem)
    return NULL;
  memset(new_mem, 0, sizeof(*new_mem));
  return new_mem;
}

char *
mnth_keyring_add(const char *key)
{
  mnth_keys *new_key;

  if (!key)
    return NULL;

  new_key = key_new_alloc0();
  if (!new_key)
    return NULL;

  memcpy(new_key->key, key, sizeof(new_key->key));

  dlist_append(GET_DLIST(&G.KEYRING), GET_DLIST(new_key));

  return new_key;
}
