

conf_data = configuration_data()
conf_data.set('IS_WINDOWS', host_machine.system() == 'windows')

config = configure_file(input: 'config.h.in',
                        output: 'config.h',
                        configuration: conf_data)

deps = [zlib_dep, libxml_dep, libxmlpp_dep, miniz_dep, outcome_dep, jpeg_dep]
inc = []
src = ['main.cpp',
       'zip.cpp',
       'args.cpp',
       'epub.cpp',
       'volumes.cpp',
       'bookmaker.cpp',
       'dtd.cpp',
       'jpeg.cpp',
       'part3.cpp',
      ]

exe = executable('dregarnuhr', src,
                 include_directories: inc,
                 dependencies: deps,
                 install: true)
