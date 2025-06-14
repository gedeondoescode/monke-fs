#define FUSE_USE_VERSION 31

#include <fuse3/fuse.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>
#define PROJECT_NAME "monke-fs"

static struct options {
  const char *filename;
  const char *contents;
  int show_help;
} options;
#define OPTION(t, p) {t, offsetof(struct options, p), 1}
static const struct fuse_opt option_spec[] = {
    OPTION("--name=%s", filename),
    OPTION("--contents=%s", contents),
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    FUSE_OPT_END
};

static void *monke_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
  (void)conn;
  cfg->kernel_cache = 1;
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
                         off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
  (void)offset;
  (void)fi;
  (void) flags;

  if (strcmp(path, "/") != 0) return -ENOENT;

  filler(buf, ".", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
  filler(buf, "..", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
  filler(buf, options.filename, NULL, 0, FUSE_FILL_DIR_DEFAULTS);

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
