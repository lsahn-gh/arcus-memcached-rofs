/**
 * mnth-fs-thread.c
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

#include "mnth-fs-thread.h"

static void *
default_fs_loop(void *arg)
{

  while (1) {
    fprintf(stderr, "MNTH -- fs thread prototype test\n");
    sleep(1);
  }

  return NULL;
}

int
mnth_fs_new_thread(void* (*cb)(void *), void *arg)
{
  pthread_t id;

  if (!cb)
    cb = default_fs_loop;
  if (pthread_create(&id, NULL, cb, arg) != 0)
    return -errno;

  return 0;
}
