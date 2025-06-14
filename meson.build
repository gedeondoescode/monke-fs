project(
  'monke-fs',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
  dependency('fuse'),
]

exe = executable(
  'monke-fs',
  'monke_fs.c',
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
