#include <fuse3/fuse.h>
#define FUSE_USE_VERSION 31

#include <errno.h>
#include <fuse/fuse.h>
#include <stdio.h>
#include <string.h>

#define PROJECT_NAME "monke-fs"

static struct options {
  const char *filename;
  const char *contents;
} options;

static void *monke_init(struct fuse_conn_info *conn,
                        struct fuse_config *config) {
  (void)conn;
  config->kernel_cache = 1;
  return NULL;
}

static int monke_getattr(const char *path, struct stat *stbuf,
                         struct fuse_file_info *file_info) {
  (void)file_info;
  int res = 0;

  memset(stbuf, 0, sizeof(struct stat));
  if (strcmp(path, "/") == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
  } else if (strcmp(path + 1, options.filename) == 0) {
    stbuf->st_mode = S_IFREG | 0444;
    stbuf->st_nlink = 1;
    stbuf->st_size = strlen(options.contents);
  } else
    res = -ENOENT;

  return res;
}

static int monke_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *file_info) {
  (void)offset;
  (void)file_info;

  if (strcmp(path, "/") != 0) return -ENOENT;

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);
  filler(buf, options.filename, NULL, 0);

  return 0;
}

static const struct fuse_operations monke_oper = {
    .init = monke_init,
    .getattr = monke_getattr,
    .readdir = monke_readdir,
};

int main(int argc, char **argv) {
  int ret;
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  ret = fuse_main(args.argc, args.argv, &monke_oper, NULL);
  fuse_opt_free_args(&args);
  return ret;
}
