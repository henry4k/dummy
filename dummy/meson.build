sources = ['core.c',
           'signal_utils.c',
           'signal_sandbox.c',
           'tap_reporter.c',
           'xml_reporter.c']
headers = ['core.h',
           'signal_sandbox.h',
           'tap_reporter.h',
           'xml_reporter.h',
           'require.h']

if add_languages('cpp', required: get_option('support_cpp'))
    sources += ['exception_sandbox.cpp',
                'inline.cpp']
    headers += ['exception_sandbox.hpp',
                'inline.hpp',
                'require.hpp',
                'bdd.hpp']
endif

public_compile_args = []
compile_args = []

if get_option('default_library') == 'shared'
    public_compile_args += ['-DDUMMY_SHARED']
    compile_args += ['-DDUMMY_BUILDING_SHARED']
endif

dummy_lib = library('dummy',
                    sources,
                    c_args:   [public_compile_args, compile_args],
                    cpp_args: [public_compile_args, compile_args],
                    gnu_symbol_visibility: 'hidden',
                    install: not meson.is_subproject())

dummy_dep = declare_dependency(link_with: dummy_lib,
                               include_directories: include_directories('..'),
                               compile_args: public_compile_args)
if not meson.is_subproject()
    install_headers(headers, subdir: 'dummy')

    pkg = import('pkgconfig')
    pkg.generate(name: 'dummy',
                 description: 'A small and flexible (unit) testing framework for C and C++.',
                 url: 'https://github.com/henry4k/dummy',
                 libraries: dummy_lib,
                 extra_cflags: public_compile_args)
endif
