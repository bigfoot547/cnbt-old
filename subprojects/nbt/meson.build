project('libnbt', 'c')

inc = include_directories('include')
libnbt_sources = []
subdir('src')

zlib = dependency('zlib')

libnbt = static_library('nbt', libnbt_sources, include_directories : inc, dependencies : zlib)
libnbt_dep = declare_dependency(include_directories : inc, link_with : libnbt)
