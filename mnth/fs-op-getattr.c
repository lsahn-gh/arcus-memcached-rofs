/**
 * fs-op-getattr.c
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
fs_op_getattr(const char *path, struct stat *stbuf,
              struct fuse_file_info *fi)
{
  int res = 0;

  (void)fi;

  memset(stbuf, 0, sizeof(*stbuf));
  if (!strcmp(path, "/")) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
  } else if (mnth_keyring_lookup(path+1)) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
  } else
    res = -ENOENT;

  return res;
}
