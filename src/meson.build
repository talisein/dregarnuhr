

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

deps = [zlib_dep, lzma_dep, libxml_dep, libxmlpp_dep, miniz_dep, outcome_dep, jpeg_dep, http_dep, ctre_dep]

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
       'jpeg.cpp',
       'omnibus.cpp',
       'utils.cpp',
       'updates.cpp',
      ]

exe = executable('dregarnuhr', src,
                 include_directories: inc,
                 dependencies: deps,
                 install: true)

test('Simple', exe, args: ['--check'])


boost_ut_args = ['-Wall',
                 '-Wextra',
                 '-Werror',
                 '-Wcast-align',
                 '-Wcast-qual',
                 '-Wdouble-promotion',
                 '-Wduplicated-branches',
                 '-Wduplicated-cond',
                 '-Wlogical-op',
                 '-Wmissing-declarations',
                 '-Wmissing-include-dirs',
                 '-Wnull-dereference',
                 '-Wold-style-cast',
                 '-Wpointer-arith',
                 '-Wredundant-decls',
                 '-Wsign-conversion',
                 '-Wswitch-enum',
                 '-Wtrampolines',
                 '-Wunused-but-set-variable',
                 '-Wunused-result',
                 '-Wuseless-cast',
                 '-Wzero-as-null-pointer-constant',
                 '-Wno-undef',
                 '-Wno-missing-declarations',
                 '-Wno-sign-conversion',
                 '-Wno-float-equal']

test_cpp_args = []
foreach arg : boost_ut_args
  if not cpp_compiler.has_argument(arg)
    if cpp_compiler.has_argument('-Wno-unknown-warning-option')
      test_cpp_args = ['-Wno-unknown-warning-option']
    endif
    break
  endif
endforeach

utils_test =  executable('utils_test', 'test_utils.cpp',
                         dependencies: [boostut_dep, deps],
                         cpp_args: test_cpp_args)

volumes_test =  executable('volumes_test', 'test_volumes.cpp', 'volumes.cpp',
                           dependencies: [boostut_dep, deps],
                           cpp_args: test_cpp_args)

test('utils', utils_test)
test('volumes', volumes_test)
