/**
 * fs-op-readdir.c
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

struct keyring_cb_data {
  fuse_fill_dir_t func;
  void *buf;
};

static void
keyring_iter_cb(mnth_keys *key, void *arg)
{
  struct keyring_cb_data *data;
  void *buf;

  if (!key || !arg)
    return;

  data = (struct keyring_cb_data *)arg;
  buf = data->buf;

  data->func(buf, key->key, NULL, 0, 0);
}

int
fs_op_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
              off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
  struct keyring_cb_data data = {
    .func = filler,
    .buf = buf
  };

  if (strcmp(path, "/"))
    return -ENOENT;

  filler(buf, ".", NULL, 0, 0);
  filler(buf, "..", NULL, 0, 0);

  mnth_keyring_iter(keyring_iter_cb, &data);

  return 0;
}
