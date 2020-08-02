/**
 * fs-op-helper.h
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

#ifndef _FS_OP_HELPER_H_
#define _FS_OP_HELPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "mnth-helper.h"

int fs_op_access(const char *path, int mask);
int fs_op_getattr(const char *path, struct stat *stbuf,
                  struct fuse_file_info *fi);
int fs_op_open(const char *path, struct fuse_file_info *fi);
int fs_op_read(const char *path, char *buf, size_t size,
               off_t offset, struct fuse_file_info *fi);
int fs_op_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info *fi,
                  enum fuse_readdir_flags flags);

#endif
