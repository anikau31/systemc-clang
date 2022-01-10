.. |systemc-clang| replace:: ``systemc-clang``

.. image:: https://github.com/anikau31/systemc-clang/workflows/CI/badge.svg
  :target: https://github.com/anikau31/systemc-clang/actions 

.. image:: https://readthedocs.org/projects/systemc-clang/badge/?version=latest
  :target: https://systemc-clang.readthedocs.io/en/latest/?badge=latest
  :alt: Documentation Status


A SystemC Parser using the Clang Front-end 
==========================================

`systemc-clang <https://github.com/anikau31/systemc-clang>`_ parses SystemC 2.3.3. It parses RTL constructs and some TLM 2.0 constructs. There is a HDL synthesis plugin that generates Verilog from SystemC RTL descriptions. 

Requirements
------------

*  `llvm/clang <https://releases.llvm.org/download.html>`_ (version 13.0.0)
*  `SystemC <http://systemc.org>`_ version 2.3.3. 
*  c++14 is required. We are using some features that necessitate c++17. Down-porting it is also possible, but not supported.

Installation
------------

*  Please see `this <https://systemc-clang.readthedocs.io/en/latest/install/install.html>`_

Plugin HDL
-----------

To compile with the HDL plugin, run cmake with the ``-DHDL=on`` flag. 
 
Documentation
--------------

* RTD: `https://systemc-clang.readthedocs.io <https://systemc-clang.readthedocs.io>`_
* Doxygen: `https://anikau31.github.io/systemc-clang/index.html <https://anikau31.github.io/systemc-clang/index.html>`_

* It is possible to build the documentation by specifying the ``-DBUILD_DOC=ON`` flag. This will provide the following targets

    * ``doxygen``: Builds Doxygen documentation. 
    * ``sphinx`` : Builds Sphinx documentation.

Tests
-------
To enable compilation of tests, run cmake with the ``-DENABLE_TESTS=on`` flag and also the ``-DSYSTEMC_DIR=<path>`` flag to pass the location for SystemC.  Without specifying the ``SYSTEMC_DIR`` path, it will not be possible to run the unit tests.

For information about running verilog conversion tests, see `this file <tests/verilog-conversion/README.md>`_.

Issues
-------

If you encounter problems, please create issues with a minimally working example that illustrates the issue.  

Development Docker image
------------------------

We build all the dependencies necessary for ``systemc-clang`` in a `Docker image <https://hub.docker.com/r/rseac/systemc-clang/tags?page=1&ordering=last_updated>`_ that you may use. 

Project Ideas
--------------

If you're interested in contributing to ``systemc-clang``, then we keep a list of interesting projects that one could approach.  Please consult `projects <https://systemc-clang.readthedocs.io/en/latest/install/projects.html>`_.

Developers
----------

Core
^^^^

* `Anirudh M. Kaushik <https://ece.uwaterloo.ca/~amkaushi/>`_
* `Hiren Patel <https://caesr.uwaterloo.ca>`_

HDL plugin
^^^^^^^^^^^
* Maya B. Gokhale
* Zhuanhao Wu

License
-------

|systemc-clang| follows the same licensing as clang. Please look at `LICENSE <LICENSE>`_.
