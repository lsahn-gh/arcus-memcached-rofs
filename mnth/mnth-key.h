/**
 * mnth-key.h
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

#ifndef _MNTH_KEY_H_
#define _MNTH_KEY_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mnth-dlist.h"

#define KEYLEN 128
#define GET_KEY(ptr) ((mnth_keys*)ptr)

enum {
  FG_OP_NONE  = 0,
  FG_OP_MRK   = (1 << 0),
  FG_LOP_MRK  = (1 << 1),
  FG_SOP_MRK  = (1 << 2),
  FG_MOP_MRK  = (1 << 3),
  FG_BOP_MRK  = (1 << 4),
};

typedef struct _mnth_keys mnth_keys;
struct _mnth_keys {
  dlist_t list;
  char key[KEYLEN];
  size_t keylen;
  size_t valsz; /* value size */
  uint64_t flag;
};

static inline void * mem_alloc0(size_t sz)
{
  void *new_mem = malloc(sz);
  if (!new_mem)
    return NULL;
  memset(new_mem, 0, sz);
  return new_mem;
}

static inline void * mnth_alloc_key(const char *key,
    size_t keylen, size_t valsz, uint64_t flag)
{
  mnth_keys *new_key;

  if (!key || !keylen)
    return NULL;

  new_key = GET_KEY(mem_alloc0(sizeof(mnth_keys)));
  if (!new_key)
    return NULL;

  if (keylen >= sizeof(new_key->key))
    keylen = sizeof(new_key->key)-1;
  memcpy(new_key->key, key, keylen);
  new_key->key[keylen] = 0; /* protect OOB */
  new_key->keylen = keylen;
  new_key->valsz = valsz;
  new_key->flag |= flag;

  return new_key;
}

#endif
