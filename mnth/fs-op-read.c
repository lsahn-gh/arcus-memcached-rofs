/**
 * fs-op-read.c
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

#include "fs-op-helper.h"

int
fs_op_read(const char *path, char *buf, size_t size,
           off_t offset, struct fuse_file_info *fi)
{
  hash_item *it;
  const char *rdata;
  const char *key;
  size_t nkey;

  key = path+1;
  nkey = strlen(key);

  it = item_get(key, nkey);
  if (it)
    rdata = item_get_data(it);
  else
    return 0;

  if (offset < it->nbytes) {
    if (offset + size > it->nbytes)
      size = it->nbytes - offset;
    memcpy(buf, rdata, size);
  } else
    size = 0;

  return size;
}
