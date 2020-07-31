/**
 * mnth-helper.h
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

#ifndef _MNTH_HELPER_H_
#define _MNTH_HELPER_H_

#include "memcached.h"

#include "mnth-keyring.h"
#include "mnth-fs-thread.h"

/* --- ARCUS APIs --- */
extern void out_string(conn *c, const char *str);


/* --- MNTH APIs --- */
#define __STRINGIFY(x) #x
#define STRINGIFY(x) __STRINGIFY(x)

#define OUTBUFSZ 256
#define mnth_string(c, str) __mnth_string(__func__, __LINE__, c, str)
static inline void __mnth_string(const char* func, const int line,
                                 conn *c, const char *str)
{
  char buf[OUTBUFSZ] = { 0, };
  snprintf(buf, OUTBUFSZ, "MNTH - %s (caller: %s:%d)", str, func, line);
  /* idc the maximum size of out_string's internal buf,
   * just focus on quick prototyping. */
  if (c)
    out_string(c, buf);
  else
    fprintf(stderr, "[%s:%d] %s", __func__, __LINE__, buf);
}

#endif
