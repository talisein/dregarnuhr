project('ctre', ['cpp'],
        license: 'Apache-2.0',
        version: '3.7.1')

ctre_dir = include_directories('single-header')

ctre_dep = declare_dependency(include_directories: ctre_dir)

meson.override_dependency('ctre', ctre_dep)
