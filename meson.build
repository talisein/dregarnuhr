project('dregarnuhr', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.1.0',
        default_options : ['c_std=c17',
                           'cpp_std=c++20',
                           'buildtype=release'])

add_global_arguments('-D_LARGEFILE64_SOURCE', language: 'c')

cpp_compiler = meson.get_compiler('cpp')
cpp_compiler.has_header_symbol('version', '__cpp_lib_filesystem', required: true)
cpp_compiler.has_header_symbol('version', '__cpp_lib_string_view', required: true)

libxmlpp_dep = dependency('libxml++-5.0',
                          version : '>=5.0.0',
                          fallback: ['libxmlpp'],
                          default_options: ['build-deprecated-api=false',
                                            'build-examples=false',
                                            'validation=false',
                                            'default_library=static'])

miniz_dep = dependency('miniz',
                       fallback: ['miniz', 'miniz_dep'],
                       default_options: ['default_library=static'])

outcome_dep = dependency('outcome-2.0',
                         fallback: ['outcome'])

subdir('src')
