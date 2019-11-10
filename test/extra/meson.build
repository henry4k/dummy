test('test',
     find_program('sh', 'bash', required: false, disabler: true),
     workdir: meson.current_source_dir(),
     protocol: 'tap',
     args: ['run-tests.sh', gen_stubs_file])

