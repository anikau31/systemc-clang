.. |systemc-clang| replace:: ``systemc-clang``

.. image:: https://github.com/anikau31/systemc-clang/workflows/CI/badge.svg
  :target: https://github.com/anikau31/systemc-clang/actions 

.. image:: https://travis-ci.com/anikau31/systemc-clang.svg?branch=master
    :target: https://travis-ci.com/anikau31/systemc-clang

.. image:: https://readthedocs.org/projects/systemc-clang/badge/?version=latest
  :target: https://systemc-clang.readthedocs.io/en/latest/?badge=latest
  :alt: Documentation Status


A SystemC Parser using the Clang Front-end 
==========================================

`systemc-clang <https://git.uwaterloo.ca/caesr-pub//systemc-clang>`_ parses SystemC 2.3.3. It parses RTL constructs and some TLM 2.0 constructs. 

Requirements
------------

*  `llvm/clang <https://releases.llvm.org/download.html>`_ (version 11.0.0)
*  `SystemC <http://systemc.org>`_ version 2.3.3. Please see `SystemC Installation notes <https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd>`_
*  c++14 is required. We are using some features that necessitate c++14. Down-porting it is also possible, but not supported.

Installation
------------

*  `Linux <docs/source/install/install-linux.rst>`_
*  `MacOS <docs/source/install/install-osx.rst>`_

Plugin HDL
-----------

To compile with the HDL plugin, run cmake with the ``-DHDL=on`` flag. 
 
Documentation
--------------

* `https://systemc-clang.readthedocs.io <https://systemc-clang.readthedocs.io>`_
* It is possible to build the documentation by specifying the ``-DBUILD_DOC=ON`` flag. This will provide the following targets
  ** ``doxygen``: Builds Doxygen documentation.
  ** ``sphinx`` : Builds Sphinx documentation.

Tests
-------
To enable compilation of tests, run cmake with the ``-DENABLE_TESTS=on`` flag and also the ``-DSYSTEMC_DIR=<path>`` flag to pass the location for SystemC.  Without specifying the `SYSTEMC_DIR` path, it will not be possible to run the unit tests.

For information about running verilog conversion tests, see `this file <tests/verilog-conversion/README.md>`_.

Issues
-------

If you encounter problems, please create issues with a minimally working example that illustrates the issue.  

Development Docker image
------------------------

We build all the dependencies necessary for ``systemc-clang`` in a `Docker image <https://hub.docker.com/r/rseac/systemc-clang/tags?page=1&ordering=last_updated>`_ that you may use. 

Developers
----------

Core
^^^^

* `Anirudh M. Kaushik <https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/>`_
* `Hiren Patel <https://caesr.uwaterloo.ca>`_

HDL plugin
^^^^^^^^^^^
* Maya B. Gokhale
* Zhuanhao Wu

License
-------

|systemc-clang| follows the same licensing as clang. Please look at `LICENSE <LICENSE>`_.
