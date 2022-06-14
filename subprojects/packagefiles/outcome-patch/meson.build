project('outcome', ['cpp'],
        license : 'Apache-2.0 OR BSL-1.0',
        version : '2.2.3')

outcome_hl_dir = include_directories('include', 'quickcpplib/repo/include')
outcome_hl_inc = [outcome_hl_dir]
outcome_hl_dep = declare_dependency(
  link_args : [],
  compile_args : [],
  include_directories : outcome_hl_inc
)
meson.override_dependency('outcome-2.0', outcome_hl_dep)

subdir('test')
