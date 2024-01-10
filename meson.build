project('dregarnuhr', ['c', 'cpp'],
        license : 'GPLv3+',
        version : '1.5.8.1',
        default_options : ['c_std=gnu17',
                           'buildtype=release',
                           'default_library=static',
                           'cpp_eh=a'])

if host_machine.system() == 'linux'
  add_global_arguments('-D_LARGEFILE64_SOURCE', language: 'c')
endif

cpp_compiler = meson.get_compiler('cpp')
if cpp_compiler.get_argument_syntax() == 'msvc'
  if cpp_compiler.has_argument('/std:c++23')
    cpp23_args = ['/std:c++23']
  else
    cpp23_args = ['/std:c++latest']
  endif
else
  if cpp_compiler.has_argument('-std=c++23')
    cpp23_args = ['-std=c++23']
  else
    cpp23_args = ['-std=c++2b']
  endif

  if cpp_compiler.get_id() == 'clang'
    cpp23_args += '-stdlib=libc++'
    cpp23_args += '-fexperimental-library'
    add_global_link_arguments('-stdlib=libc++', language: 'cpp')

    if get_option('buildtype') == 'debug'
      cpp23_args += '-gdwarf-4'
      add_global_arguments('-gdwarf-4', language: 'c')
    endif
  endif

endif

message('Adding cpp23 args: ', cpp23_args)
add_global_arguments(cpp23_args, language: 'cpp')

cpp_compiler.has_header_symbol('version', '__cpp_lib_filesystem', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_string_view', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_ranges', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_source_location', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_concepts', required: true, args: cpp23_args)
cpp_compiler.compute_int('__cpp_concepts', prefix: '#include<version>', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_integer_comparison_functions', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_expected', required: true, args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_size_t_suffix', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_ranges_zip', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_stacktrace', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_spanstream', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_flat_map', args: cpp23_args)
cpp_compiler.has_header_symbol('version', '__cpp_lib_format', args: cpp23_args)

has_chrono = 1 == cpp_compiler.compute_int('__cpp_lib_chrono >= 201907L', prefix: '#include<chrono>', args: cpp23_args)

zlib_dep = dependency('zlib', fallback: ['zlib'])

# Fedora libjpeg-turbo-devel, Ubuntu libjpeg-turbo8-dev
png_dep = dependency('libpng', fallback: ['libpng'])

# Fedora libjpeg-turbo-devel, Ubuntu libjpeg-turbo8-dev
jpeg_dep = dependency('libjpeg', version: '>=2.1.0', fallback: ['libjpeg-turbo'])

# Fedora miniz-devel. Ubuntu unpackaged.
# Note: Fedora's package has no pkgconfig, so meson doesn't find it.
miniz_dep = cpp_compiler.find_library('miniz',
                                      has_headers: ['miniz.h'],
                                      required: false)

if not miniz_dep.found()
  miniz_dep = dependency('miniz', fallback: ['miniz', 'miniz_dep'])
endif

# Unpackaged
if not cpp_compiler.has_header('outcome/outcome.hpp', args: cpp23_args)
  outcome_dep = dependency('outcome-2.0', fallback: ['outcome'])
else
  outcome_dep = dependency('', required: false)
endif

# zlib is needed by libxml2. Specify fallback to our wrap to avoid strawberry perl on windows
lzma_dep = dependency('liblzma', fallback: ['liblzma'])

# Fedora libxml2-devel, Ubuntu libxml2-dev
libxml_dep = dependency('libxml-2.0', fallback: ['libxml2'])

# Fedora cpp-httplib-devel, Ubuntu libcpp-httplib-dev
# Note: Ubuntu is not header-only; it has a shared library. So for building
# distributable, we do not use the ubuntu package to keep the oddball dynamic
# dependencies low.

if cpp_compiler.get_id() == 'clang'
http_dep = dependency('httplib', 'cpp-httplib',
                      fallback: ['cpp-httplib', 'cpp_httplib_dep'],
                      default_options: ['cpp-httplib_compile=false',
                                        'cpp-httplib_brotli=disabled'])
else
http_dep = dependency('httplib', 'cpp-httplib',
                      fallback: ['cpp-httplib', 'cpp_httplib_dep'],
                      default_options: ['cpp-httplib_compile=true',
                                        'cpp-httplib_brotli=disabled'])
endif
# Fedora libxml++-devel. Ubuntu doesn't have a -5.0 package.
# Note: I submitted some patches upstream, so the wrap has better performance
# until there is a 5.0.2 release
libxmlpp_dep = dependency('libxml++-5.0',
                          version : '>=5.0.2',
                          fallback: ['libxmlpp'],
                          default_options: ['build-deprecated-api=false',
                                            'build-examples=false',
                                            'validation=false',
                                            'maintainer-mode=false'])


if not has_chrono
  # Fedora date-devel, Ubuntu libhowardhinnant-date-dev
  # Note: Only needed until GCC implements c++20 <chrono>. Maybe in 13?
  date_dep = dependency('date', 'howardhinnant-date',
                        fallback: ['hinnant-date', 'date_dep'],
                        default_options: ['use_manual_tzdb=true'])
endif

boostut_dep = dependency('ut',
                         method: 'cmake',
                         modules: 'Boost::ut',
                         fallback: ['boostut', 'boostut_dep'])

add_project_arguments('-DBOOST_UT_DISABLE_MODULE', language: 'cpp')


ctre_dep = dependency('ctre',
                      fallback: ['ctre'])

magic_enum_dep = dependency('magic_enum',
			    version: '>=0.9.3',
                            fallback: ['magic_enum'])

subdir('src')
subdir('test')
