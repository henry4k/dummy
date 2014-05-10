dummy
=====

A small and flexible (unit) testing framework for C and C++.

Features:
- small core: Everything except `core.h/c` is optional.
- portable: It just depends on the C standard library.
- extensible: Reporters, API porcelain and sandboxes can be customized.


Description
-----------

At its core, it just consists of a small set of functions, written in C.  
Since small things tend to break less often than big contraptions.  
And a test system is not the kind of code that may break.

      [porcelain]
          ||
          \/
        [core] => [sandbox] => [test]
          ||
          \/
      [reporter]

At first, you need to initialize dummy and register some tests.  
Then you can tell it to run those tests:
A sandbox is created for every test and catches errors during the execution.  
Any events that happen while running the tests are passed to the reporter.  
And that's all there is to it - everything else is under your control.

Reporters, API porcelain and sandboxes can be plugged onto it, just as you need it.  
Some are already provided, but it shouldn't be hard to write your own.

These are currently provided by dummy:

*Reporters*:
- `xml_reporter`: Prints a report in an JUnit complatible XML format.
- `tap_reporter`: Reports using Perls 'test anything protocol' (TAP).

*Sandboxes*:
- `signal_sandbox`: Catches fatal errors like segmentation violation or illegal instruction.
- `exception_sandbox`: Besides signals, this also catches C++ exceptions.

*API porcelain*:
- `bdd`: "Behaviour driven design" - Lets you define tests BDD style. See `test/bdd.cpp` for example.
- `inline`: Register tests as you define their functions. See `test/inline.c` for example.
- `require`: Check if an expression is true. (Like `assert()`, but tailored for dummy.)


Usage
-----

Just copy the parts you need into your project,
since anything except `core.h/c` is optional.

When testing C++ code using the `exception_sandbox`,
you need to compile `core.c` with a C++ compiler.
Because C++ exceptions can't be thrown through C code.

For code examples, just take a look at the test directory.


Licence
-------

*TODO*
But probably a very permissive licence,
that allows you to copy and modify the code to your needs.
(MIT,BSD,zlib .. ?)

Henry Kielmann
