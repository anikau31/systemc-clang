.. highlight:: console

====================
Using systemc-clang
====================

After successfully compiling `systemc-clang`, it is possible to run the tool, but it requires several additional arguments.
Assuming that the current path is in the directory that contains the ``systemc-clang`` binary, the tool can be run as follows.
::
  $ ./systemc-clang test.cpp -- __STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  \
  -I<path-to-llvm-install>/lib/clang/10.0.0/include/ -I/usr/include -I$<path-to-systemc>/include \
  -x c++ -w -c

It is important to note that ``systemc-clang`` requires the arguments to pass into it the location for the ``clang`` includes, and ``SystemC`` includes.

Useful scripts to execute ``systemc-clang``
--------------------------------------------

The distribution provides the following useful scripts to execute ``systemc-clang``.

* ``run.sh``: executes ``systemc-clang``.
* ``run-gdb.sh``: executes gdb with ``systemc-clang``.

Both of these scripts require setting some environment variables. 
Most of these are defined in the following scripts to make it easier for users.
* ``paths.sh``: A bash shell script to set the appropriate environment variables.
* ``paths.fish``: A fish shell script to set the appropriate environment variables.

Specifically, these scripts require the user to set the following variables. 
These can be changed in the file itself. 

* ``LLVM_INSTALL_DIR``: This points to the directory where clang is installed. 
* ``SYSTEMC``: This points to the directory where SystemC is installed.
* ``SYSTEMC_CLANG_BUILD_DIR``: This points to where ``systemc-clang`` was built.

Once the appropriate environment variables are set, the scripts used to run ``systemc-clang`` can be executed successfully.
::
  $ scripts/run.sh test.cpp
