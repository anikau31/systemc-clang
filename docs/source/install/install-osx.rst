.. highlight:: console

Installing systemc-clang on OSX
---------------------------------
We provide instructions for installation using OSX.
You can install LLVM and Clang via source or via binaries. 

* systemc-clang requires llvm-config and by default does not come with the llvm package preinstalled on MacOS. 
* Download the clang pre built binaries from [here](http://llvm.org/releases/download.html) for LLVM 7 or higher.
* Untar the package into a suitable location, and modify the ``path.sh`` bash script in the systemc-clang folder to point to llvm-config, which will be found in the bin directory of the uncompressed binary. 

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
^^^^^^^^^^^^^^^^^^^^^^^^^^

Copy the compiled binary in the bin folder of ``LLVM_BUILD_DIR``
::
   $ cp systemc-clang $LLVM_BUILD_DIR/bin

Set the ``SYSTEMC`` variable.
::
   $ export SYSTEMC=<path-to-systemc-install>

Execute it on a simple example.  You should see some (debug) information print out on the screen.  Hope to make this more meaningful in the future.
::
   $ $LLVM_BUILD_DIR/bin/systemc-clang ../systemc-clang/tests/module1.hpp -- \
   -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS  -I/usr/include  \
   -I$SYSTEMC/include  -x c++ -w -c
