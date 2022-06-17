project('dregarnuhr', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.1.0',
        default_options : ['c_std=gnu17',
                           'cpp_std=gnu++20',
                           'buildtype=release'])

if host_machine.system() == 'linux'
  add_global_arguments('-D_LARGEFILE64_SOURCE', language: 'c')
endif

cpp_compiler = meson.get_compiler('cpp')
cpp_compiler.has_header_symbol('version', '__cpp_lib_filesystem', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_string_view', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_ranges', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_source_location', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_concepts', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_integer_comparison_functions', required: true)

zlib_dep = dependency('zlib', fallback: ['zlib'])

libxml_dep = dependency('libxml-2.0', fallback: ['libxml2'])

libxmlpp_dep = dependency('libxml++-5.0',
                          version : '>=5.0.0',
                          fallback: ['libxmlpp'],
                          default_options: ['build-deprecated-api=false',
                                            'build-examples=false',
                                            'validation=false',
                                            'maintainer-mode=false'])

miniz_dep = dependency('miniz',
                       fallback: ['miniz', 'miniz_dep'])

outcome_dep = dependency('outcome-2.0',
                         fallback: ['outcome'])

subdir('src')
