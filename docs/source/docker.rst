======
Use Docker
======

We provide a Docker image that can be used to build ``systemc-clang``.  

First clone the `systemc-clang <https://github.com/anikau31/systemc-clang>`_ repository. 
::
  $ git clone git@github.com:anikau31/systemc-clang.git

Then pull the systemc-clang Docker image.
::
  $ docker pull rseac/systemc-clang:clang-13.0.0

Start the Docker image while providing the locally cloned copy of ``systemc-clang`` as a volume to load. 
::
  $ docker run -it -v <full-path-to-systemc-clang-cloned-dir>:/systemc-clang --name systemc-clang  rseac/systemc-clang:clang-13.0.0

Start the compile process once you enter the Docker terminal.
::
  $ cmake /systemc-clang -DHDL=ON -DENABLE_TESTS=ON -DENABLE_VERILOG_TESTS=ON -G Ninja && ninja && ctest

Once you have started the Docker instance, you can re-join it as follows.
::
  $ docker start -ia systemc-clang










