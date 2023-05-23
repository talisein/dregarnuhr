
test('Simple', exe, args: ['--check'])

volumes_test =  executable('volumes_test', 'volumes.cpp',
                           dependencies: [boostut_dep, dep])
test('volumes', volumes_test)

utils_test =  executable('utils_test', 'utils.cpp',
                         dependencies: [boostut_dep, dep])
test('utils', utils_test)

args_test = executable('args_test', 'args.cpp',
                       dependencies: [boostut_dep, dep])
test('args', args_test)

jq = find_program('jq', required: false)

if jq.found()
  test('dieties', jq, args: ['.', '../res/dieties.json'])
  test('retainers', jq, args: ['.', '../res/retainers.json'])
endif
