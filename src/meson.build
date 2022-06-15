

deps = [zlib_dep, libxml_dep, libxmlpp_dep, miniz_dep, outcome_dep]
inc = []
src = ['main.cpp', 'zip.cpp', 'args.cpp', 'epub.cpp']

exe = executable('dregarnuhr', src,
                 include_directories: inc,
                 dependencies: deps,
                 install: true)
