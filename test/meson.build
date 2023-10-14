

test('Simple', exe, args: ['--check'])

ut_args = ['-r', 'junit', '-o']
volumes_test =  executable('volumes_test', 'volumes.cpp',
                           dependencies: [boostut_dep, dep])
test('volumes', volumes_test,
     args: [ut_args, 'volumes-junit.xml'])

utils_test =  executable('utils_test', 'utils.cpp',
                         dependencies: [boostut_dep, dep])
test('utils', utils_test,
     args: [ut_args, 'utils-junit.xml'])

args_test = executable('args_test', 'args.cpp',
                       dependencies: [boostut_dep, dep])
test('args', args_test,
     args: [ut_args, 'args-junit.xml'])
