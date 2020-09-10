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

#include "mnth-helper.h"
#include "fs-op-helper.h"

static void *
fs_op_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
  (void)conn;

  cfg->use_ino = 1;
  cfg->entry_timeout = 0;
  cfg->attr_timeout = 0;
  cfg->negative_timeout = 0;

  return NULL;
}

static const struct fuse_operations fs_ops = {
  .init = fs_op_init,
  .access = fs_op_access,
  .getattr = fs_op_getattr,
  .open = fs_op_open,
  .read = fs_op_read,
  .readdir = fs_op_readdir,
};

static void *
default_fs_loop(void *arg)
{
  char* fake_argv[] = {
    "memcached",
    "-f",
    "-o",
    "auto_unmount",
    "/tmp/mpoint",
  };

  fuse_main(sizeof(fake_argv)/sizeof(fake_argv[0]),
            fake_argv, &fs_ops, NULL);

  return NULL;
}

pthread_t
mnth_fs_thread_new(void* (*cb)(void *), void *arg)
{
  pthread_t id;

  if (!cb)
    cb = default_fs_loop;
  if (pthread_create(&id, NULL, cb, arg) != 0)
    return -errno;

  return id;
}
