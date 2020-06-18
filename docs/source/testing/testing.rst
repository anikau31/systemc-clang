Testing
=======

systemc-clang uses `Catch2 <https://github.com/catchorg/Catch2>`_ to implement tests.
There are several examples under ``tests/`` that can be viewed.
This part of the documentation will go through creating a simple test, and integrating it into the build system.

Unit test for ``counter`` example
----------------------------------

In order to run systemc-clang from within a C++ program, we employ ASTUnit to construct the AST for the provided source code.

Boilerplate
^^^^^^^^^^^


