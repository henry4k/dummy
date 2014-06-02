#!/usr/bin/env python3.3

import re
import os.path
import argparse

parser = argparse.ArgumentParser(description='Generate stubs for c functions.')
parser.add_argument('module_header_name', metavar='module_header')
args = parser.parse_args()

class CParameter():
    def __init__(self, name, type):
        self.name = name
        self.type = type

    def __str__(self):
        return '{} {}'.format(
            self.type,
            self.name)

    def __repr__(self):
        return "CParameter(name='{}', type='{}')".format(
            self.name,
            self.type)

class CFunction():
    def __init__(self, name, return_type, parameters):
        self.name = name
        self.return_type = return_type
        self.parameters = parameters

    def __str__(self):
        return '{} {}({})'.format(
            self.return_type,
            self.name,
            ', '.join(str(p) for p in self.parameters))

    def __repr__(self):
        return "CFunction(name='{}', return_type='{}', parameters=[{}])".format(
            self.name,
            self.return_type,
            ', '.join(repr(p) for p in self.parameters))

parameter_pattern = re.compile(r'''
    (?P<type>[^ ].+[^ ])
    \s+
    (?P<name>[^ ]+)
    ''', re.VERBOSE)

function_pattern = re.compile(r'''
    (?P<return_type> [^ ].+[^ ] )
    \s+
    (?P<name> [^ ]+ )
    \s*
    \(
    (?P<parameters> .* )
    \)
    \s*
    ;
    ''', re.VERBOSE)

def parse_cparameters(parameters):
    for parameter_string in parameters.split(','):
        parameter_match = parameter_pattern.search(parameter_string)
        if parameter_match:
            name = parameter_match.group('name')
            type = parameter_match.group('type')
            yield CParameter(name=name, type=type)

def try_parse_cfunction(function_string):
    function_match = function_pattern.search(function_string)
    if function_match:
        name = function_match.group('name')
        return_type = function_match.group('return_type')
        parameters = list(parse_cparameters(function_match.group('parameters')))
        return CFunction(name=name,
                         return_type=return_type,
                         parameters=parameters)
    else:
        return None

def get_cfunction_stub_pointer_name(function):
    return function.name+'_stub'

def write_cfunction_stub_pointer(function, file, extern):
    stub_pointer_template = None
    if extern:
        stub_pointer_template = \
            'extern {return_type} (*{pointer_name})({parameters});\n'
    else:
        stub_pointer_template = \
            '{return_type} (*{pointer_name})({parameters}) = NULL;\n'

    pointer_name = get_cfunction_stub_pointer_name(function)
    parameters = ', '.join(p.type for p in function.parameters)
    file.write(stub_pointer_template.format(
        return_type=function.return_type,
        pointer_name=pointer_name,
        parameters=parameters))

def write_cfunction_stub_implementation(function, file):
    implementation_template = \
'''{return_type} {name}({parameters})
{{
    if(!{pointer_name})
        dummyAbortTest("Brabbel");
    return {pointer_name};
}}
'''
    pointer_name = get_cfunction_stub_pointer_name(function)
    parameters = ', '.join(str(p) for p in function.parameters)
    file.write(implementation_template.format(
        return_type=function.return_type,
        name=function.name,
        parameters=parameters,
        pointer_name=pointer_name))

def get_stub_header_name(name):
    return name+'_stub.h'

def get_stub_implementation_name(name):
    return name+'_stub.c'

def write_stub_header(name, header_name, functions):
    file_name = get_stub_header_name(name)
    with open(file_name, 'w', encoding='UTF-8') as file:
        file.write('#include "{}"\n'.format(header_name))
        file.write('\n')
        for function in functions:
            write_cfunction_stub_pointer(function, file, extern=True)

def write_stub_implementation(name, header_name, functions):
    file_name = get_stub_implementation_name(name)
    with open(file_name, 'w', encoding='UTF-8') as file:
        file.write('#include <stddef.h> // NULL\n')
        file.write('#include "{}"\n'.format(get_stub_header_name(name)))
        file.write('\n')
        for function in functions:
            write_cfunction_stub_pointer(function, file, extern=False)
            write_cfunction_stub_implementation(function, file)
            file.write('\n')



module_header_name = args.module_header_name
module_name = os.path.splitext(os.path.basename(module_header_name))[0]

with open(module_header_name, 'r', encoding='UTF-8') as file:
    functions = []
    for line in file:
        function = try_parse_cfunction(line)
        if function:
            functions.append(function)
        write_stub_header(name=module_name,
                          header_name=module_header_name,
                          functions=functions)
        write_stub_implementation(name=module_name,
                                  header_name=module_header_name,
                                  functions=functions)