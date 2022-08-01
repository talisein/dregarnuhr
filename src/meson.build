

conf_data = configuration_data()
conf_data.set('IS_WINDOWS', host_machine.system() == 'windows' ? 'true' : 'false')
conf_data.set10('HAVE_CHRONO', has_chrono)

if cpp_compiler.has_header_symbol('ios', 'std::ios_base::noreplace')
  conf_data.set('NOREPLACE', 'std::ios_base::noreplace')
elif cpp_compiler.has_header_symbol('ios', 'std::ios_base::__noreplace')
  conf_data.set('NOREPLACE', 'std::ios_base::__noreplace')
elif cpp_compiler.has_header_symbol('ios', 'std::ios_base::_Noreplace')
  conf_data.set('NOREPLACE', 'std::ios_base::_Noreplace')
else
  conf_data.set('NOREPLACE', 'std::ios_base::binary')
endif

config = configure_file(input: 'config.h.in',
                        output: 'config.h',
                        configuration: conf_data)

deps = [libxml_dep, libxmlpp_dep, miniz_dep, outcome_dep, jpeg_dep, http_dep]

if not has_chrono
  deps += date_dep
endif

inc = []
src = ['main.cpp',
       'zip.cpp',
       'args.cpp',
       'epub.cpp',
       'volumes.cpp',
       'bookmaker.cpp',
       'dtd.cpp',
       'jpeg.cpp',
       'part1.cpp',
       'part2.cpp',
       'part3.cpp',
       'part4.cpp',
       'omnibus.cpp',
       'utils.cpp',
       'updates.cpp',
      ]

exe = executable('dregarnuhr', src,
                 include_directories: inc,
                 dependencies: deps,
                 install: true)

test('Simple', exe, args: ['--check'])
