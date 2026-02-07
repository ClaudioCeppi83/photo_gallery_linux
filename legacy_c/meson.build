project('photo-gallery', 'c',
  version: '0.3.0',
  meson_version: '>= 0.62.0',
  default_options: [ 'warning_level=2', 'werror=false', 'c_std=gnu11' ],
)

i18n = import('i18n')
gnome = import('gnome')

gtk_dep = dependency('gtk4')
adw_dep = dependency('libadwaita-1')

inc = include_directories('src')

sources = [
  'src/main.c',
  'src/gallery.c',
  'src/interface.c',
]

executable('photo_gallery',
  sources,
  include_directories: inc,
  dependencies: [gtk_dep, adw_dep],
  install: true,
)
