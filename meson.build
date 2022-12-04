project('cnbt', 'c')

libnbt_proj = subproject('nbt')
libnbt_dep = libnbt_proj.get_variable('libnbt_dep')

cnbt_sources = []
subdir('src')
executable('cnbt', cnbt_sources,
    include_directories : ['include'],
    dependencies : libnbt_dep)
