project('dregarnuhr', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '0.9.10',
        default_options : ['c_std=gnu17',
                           'cpp_std=gnu++20',
                           'buildtype=release',
                           'cpp_eh=a'])

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
cpp_compiler.has_header_symbol('version', '__cpp_lib_ranges_zip')
cpp_compiler.has_header_symbol('version', '__cpp_lib_stacktrace')

jpeg_dep = dependency('libjpeg', version: '>=2.1.0', fallback: ['libjpeg-turbo'])

zlib_dep = dependency('zlib', fallback: ['zlib'])

miniz_dep = dependency('miniz', fallback: ['miniz', 'miniz_dep'])

outcome_dep = dependency('outcome-2.0', fallback: ['outcome'])

libxml_dep = dependency('libxml-2.0', fallback: ['libxml2'])

libxmlpp_dep = dependency('libxml++-5.0',
                          version : '>=5.0.0',
                          fallback: ['libxmlpp'],
                          default_options: ['build-deprecated-api=false',
                                            'build-examples=false',
                                            'validation=false',
                                            'maintainer-mode=false'])

cmake = import('cmake')
opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({
  'BUILD_CURL_EXE': false,
  'BUILD_TESTING': false,
  'BUILD_SHARED_LIBS': false,
})
opt_var.set_install(false)

curl_proj = cmake.subproject('libcurl', options: opt_var)
message(curl_proj.target_list())
curl_dep = curl_proj.dependency('libcurl')

date_dep = dependency('date',
                      fallback: ['hinnant-date', 'tz_dep'],
                      default_options: ['default_library=static'])

subdir('src')
