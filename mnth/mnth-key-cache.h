/**
 * mnth-key-cache.h
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

#ifndef _MNTH_KEY_CACHE_H_
#define _MNTH_KEY_CACHE_H_

#include "mnth-key.h"

char * mnth_key_cache_add(mnth_keys *new_key);
char * mnth_key_cache_lookup(const char *key);
char * mnth_key_cache_rm(mnth_keys *key);
void mnth_key_cache_iter(void (*cb)(mnth_keys *key, void*), void* arg);
void mnth_key_cache_dump(void);

#endif
