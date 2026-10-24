#include "vfs.h"
#include <assert.h>
#include <stdio.h>

int main() {
  vfsInit();
  Stream *s = vfsOpen("test.txt", "wb");
  assert(s);
  vfsWrite(s, "hello", 5);
  vfsClose(s);

  // now read
  s = vfsOpen("./test.txt", "rb");
  char buf[10];
  vfsSeek(s, 0, 2);
  size_t size = vfsTell(s);
  vfsSeek(s, 0, 0);
  vfsRead(s, buf, size);
  assert(vfsEof(s));
  buf[size] = '\0';
  printf("%s\n", buf);
  assert(strcmp(buf, "hello") == 0);
  vfsClose(s);

  assert(vfsRemove("./test.txt") == 0);
  assert(vfsRemove("./test.txt") == -1);
  assert(!vfsOpen("./test.txt", "r"));
  assert(vfsOpen("./test.txt", "w"));
  assert(vfsOpen("./test.txt", "r"));
  assert(vfsRemove("./././test.txt") == 0);

  s = vfsOpen("test.txt", "rb");
  assert(s == NULL);

  assert(vfsCreate("test1.txt") == 0);
  assert(vfsCreate("test2.txt") == 0);
  assert(vfsCreate("subdir") == 0);
  assert(vfsCreate("subdir/test1.txt") == 0);
  assert(vfsExists("test1.txt"));
  assert(vfsExists("test2.txt"));
  assert(vfsExists("subdir"));
  assert(vfsExists("subdir/test1.txt"));
  assert(!vfsExists("subdir/test2.txt"));
  assert(vfsExists("subdir/../subdir/test1.txt"));

  // Boundary checks: read past EOF should safely return 0
  s = vfsOpen("test1.txt", "rb");
  assert(s);
  vfsSeek(s, 100, 0);
  assert(vfsRead(s, buf, sizeof(buf)) == 0);
  vfsClose(s);

  // Path with > 32 segments should safely fail without stack overflow
  assert(!vfsExists("a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z/1/2/3/4/5/6/7"));
  assert(!vfsExists("a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z/1/2/3/4/5/6/7/"));

  // Filename longer than 31 chars should safely truncate without corrupting adjacent Node pointers
  assert(vfsCreate("long_file_name_exceeding_node_filename_limit.txt") == 0);

  printf("OK\n");
}