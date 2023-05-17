

conf_data = configuration_data()
conf_data.set('IS_WINDOWS', host_machine.system() == 'windows' ? 'true' : 'false')
conf_data.set10('HAVE_CHRONO', has_chrono)

if cpp_compiler.has_header_symbol('ios', 'std::ios_base::noreplace', args: cpp23_args)
  conf_data.set('NOREPLACE', 'std::ios_base::noreplace')
elif cpp_compiler.has_header_symbol('ios', 'std::ios_base::__noreplace', args: cpp23_args)
  conf_data.set('NOREPLACE', 'std::ios_base::__noreplace')
elif cpp_compiler.has_header_symbol('ios', 'std::ios_base::_Noreplace', args: cpp23_args)
  conf_data.set('NOREPLACE', 'std::ios_base::_Noreplace')
else
  conf_data.set('NOREPLACE', 'std::ios_base::binary')
endif

config = configure_file(input: 'config.h.in',
                        output: 'config.h',
                        configuration: conf_data)

deps = [zlib_dep, lzma_dep, libxml_dep, libxmlpp_dep, miniz_dep, outcome_dep, jpeg_dep, http_dep, ctre_dep, png_dep, magic_enum_dep]

if not has_chrono
  deps += date_dep
endif

inc = []
lib_srcs = ['zip.cpp',
       'args.cpp',
       'epub.cpp',
       'volumes.cpp',
       'chapter_type.cpp',
       'volume_definition.cpp',
       'bookmaker.cpp',
       'jpeg.cpp',
       'png_reader.cpp',
       'omnibus.cpp',
       'utils.cpp',
       'updates.cpp',
           ]

lib = static_library('dregarnuhr', lib_srcs,
                     dependencies: deps,
                     install: false,
                     include_directories: inc)

dep = declare_dependency(link_with: lib,
                         include_directories: include_directories('./'),
                         dependencies: deps)

exe = executable('dregarnuhr', 'main.cpp',
                 include_directories: inc,
                 dependencies: dep,
                 install: true)
