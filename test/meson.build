test_deps = [dummy_dep,
             declare_dependency(link_with: library('test_reporter',
                                                   'test_reporter.c',
                                                   dependencies: dummy_dep),
                                include_directories: include_directories('.'))]

c_tests = ['core',
           'require',
           'tap_reporter',
           'xml_reporter']
cpp_tests = ['cpp',
             'inline',
             'bdd',
             'sandbox']
test_targets = []

foreach name : c_tests
     test_targets += executable(name,
                                name+'.c',
                                dependencies: test_deps)
endforeach

if add_languages('cpp', required: get_option('support_cpp'))
    foreach name : cpp_tests
         test_targets += executable(name,
                                    name+'.cpp',
                                    dependencies: test_deps)
    endforeach
endif

test('test',
     find_program('sh', 'bash', required: false, disabler: true),
     workdir: meson.current_source_dir(),
     protocol: 'tap',
     depends: test_targets,
     args: ['run-tests.sh']+test_targets)

