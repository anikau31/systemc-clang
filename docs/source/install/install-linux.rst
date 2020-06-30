.. highlight:: console

Installing systemc-clang on Linux
---------------------------------

We provide instructions for installation using Linux. 
You can install LLVM and Clang via source or via binaries. 

Building LLVM/Clang from source 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Follow the instructions to `install clang <http://clang.llvm.org/get_started.html>`_. 
Notice that you should also install LLVM as it is needed for compilation of the stand-along program. A suggestion is to install LLVM/clang in a location that is accessible by the user.
Note that the clang-llvm version systemc-clang supports is version 7.0.0 and higher.  

Building systemc-clang 
^^^^^^^^^^^^^^^^^^^^^^

Clone systemc-clang repository. 
::
$ git clone https://github.com/anikau31/systemc-clang.git

Create a build directory for this stand-alone program.
::
$ mkdir systemc-clang-build

Enter the cloned repository folder.
::
$ cd systemc-clang

Edit the path for the ``LLVM_INSTALL_DIR`` in ``paths.sh``, and set it to the appropriate path where LLVM/clang was installed.

Source the paths file (assuming bash shell).  This should setup certain environment variables that ``cmake`` will use.
::
$ source scripts/paths.sh

Go to the build directory.
::
$ cd ../systemc-clang-build

Run ``cmake``.
::
$ cmake ../systemc-clang

Alternatively, you could use cmake to generate `Ninja <https://ninja-build.org>`_ build scripts.
::
$ cmake ../systemc-clang -G Ninja

If there are no errors, then the configuration was successful.  So, you can go ahead and compile the stand-alone program.
::
$ make

If using ``ninja``:
::
$ ninja

Testing on a small example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Copy the compiled binary in the bin folder of ``LLVM_INSTALL_DIR``.
::
$ cp systemc-clang $LLVM_INSTALL_DIR/bin

Set the ``SYSTEMC`` variable.
::
$ export SYSTEMC=<path-to-systemc-install>

Execute it on a simple example.  You should see some (debug) information print out on the screen.  Hope to make this more meaningful in the future.
::
$ $LLVM_INSTALL_DIR/bin/systemc-clang ../systemc-clang/tests/module1.hpp -- \
-D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/include  \
-I$SYSTEMC/include  -x c++ -w -c


