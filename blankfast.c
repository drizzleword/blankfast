//  Copyright 2024 Rùnag project contributors
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
  #include <linux/fs.h>
  #include <sys/ioctl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

#define BLOCKSIZE 4096

static char data[BLOCKSIZE];
static char emptyblock[BLOCKSIZE];

int main(int argc, char *argv[]){
  int file_descriptor;
  int open_flags = O_RDWR;

  ssize_t read_count;

  unsigned long long int total_read_count = 0;
  unsigned long long int total_write_count = 0;

  setlocale(LC_NUMERIC, "");

  if (argc != 2) {
    fprintf(stderr, "Blankfast will overwrite with zeros all 4K-blocks that contains non-zero values.\n");
    fprintf(stderr, "Before you proceed please understand that it will destroy your data.\n\n");
    fprintf(stderr, "Please specify file path as an argument. It could be a regular or device file.\n");
    return 1;
  }

  #ifdef __linux__
    open_flags |= O_EXCL;
  #endif

  if ((file_descriptor = open(argv[1], open_flags)) == -1) {
    fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
    return 1;
  }

  // probably I don't need to zero memory by myself, but just to be sure
  memset(emptyblock, 0x00, BLOCKSIZE);

  for(;;) {
    if ((read_count = read(file_descriptor, data, BLOCKSIZE)) == -1) {
      fprintf(stderr, "Read error: %s\n", strerror(errno));
      return 1;
    }

    if (read_count == 0) {
      break;
    }

    total_read_count += read_count;

    if (memcmp(data, emptyblock, read_count) != 0) {

      if (lseek(file_descriptor, (0 - read_count), SEEK_CUR) == -1) {
        fprintf(stderr, "Read/write offset reposition failed: %s\n", strerror(errno));
        return 1;
      }

      if (write(file_descriptor, emptyblock, read_count) == -1) {
        fprintf(stderr, "Write error: %s\n", strerror(errno));
        return 1;
      }

      total_write_count += read_count;
    }
  }

  if (fsync(file_descriptor) == -1) {
    fprintf(stderr, "Storage device transfer synchronization error: %s\n", strerror(errno));
    return 1;
  }

  #ifdef __linux__
    struct stat file_stat;

    if (fstat(file_descriptor, &file_stat) == -1) {
      fprintf(stderr, "Unable to obtain file stat: %s\n", strerror(errno));
      return 1;
    }

    if (S_ISBLK(file_stat.st_mode)) {
      if (ioctl(file_descriptor, BLKRRPART) == -1) {
        fprintf(stderr, "Warning: block device partition table re-read failed: %s\n", strerror(errno));
      }
    }
  #endif

  if (close(file_descriptor) == -1) {
    fprintf(stderr, "Unable to close file descriptor: %s\n", strerror(errno));
    return 1;
  }

  fprintf(stdout, "%llu bytes (%'llu MiB) read\n%llu bytes (%'llu MiB) written\n", 
    total_read_count, total_read_count / (1024 * 1024),
    total_write_count, total_write_count / (1024 * 1024)
  );

  return 0;
}
