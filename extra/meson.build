gen_stubs_file = files('gen_stubs.py')

if not meson.is_subproject()
    install_data('gen_stubs.py',
                 rename: 'dummy_gen_stubs',
                 install_dir: get_option('bindir'),
                 install_mode: 'rwxr-xr-x')
endif
