.. |systemc-clang| replace:: ``systemc-clang``

.. image:: https://github.com/anikau31/systemc-clang/workflows/CI/badge.svg
  :target: https://github.com/anikau31/systemc-clang/actions 

.. image:: https://readthedocs.org/projects/systemc-clang/badge/?version=latest
  :target: https://systemc-clang.readthedocs.io/en/latest/?badge=latest
  :alt: Documentation Status


SCCL: An open-source SystemC to RTL translator
==========================================

`SCCL <https://github.com/anikau31/systemc-clang>`_  is an open-source tool that translates synthesizable SystemC to SystemVerilog RTL. This project started out as front-end for parsing and analyzing SystemC models, and it has evolved into a translator with the addition of a HDL synthesis plugin.

Requirements
------------

*  `llvm/clang <https://releases.llvm.org/download.html>`_ (version 15.0.6)
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

We build all the dependencies necessary for ``SCCL`` in a `Docker image <https://hub.docker.com/r/rseac/systemc-clang/tags?page=1&ordering=last_updated>`_ that you may use.  Follow these `Docker SCCL instructions <docs/source/docker.rst>`_ to use the Docker image for building ``SCCL``, and running benchmarks.  

Project Ideas
--------------

If you're interested in contributing to ``SCCL``, then we keep a list of interesting projects that one could approach.  Please consult `projects <https://systemc-clang.readthedocs.io/en/latest/projects.html>`_.

Publications
---------------

Z. Wu, M. Gokhale, S. Lloyd and H. Patel, `SCCL: An open-source SystemC to RTL translator <https://caesr.uwaterloo.ca/assets/pdfs/wu_23_sccl_fccm.pdf>`_, 2023 IEEE 31st Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM), Marina Del Rey, CA, USA, 2023, pp. 23-33, doi: 10.1109/FCCM57271.2023.00012.

Developers
----------

Core
^^^^

* `Anirudh M. Kaushik <https://ece.uwaterloo.ca/~amkaushi/>`_
* `Hiren Patel <https://caesr.uwaterloo.ca>`_

HDL plugin
^^^^^^^^^^^
* `Zhuanhao Wu <https://zhuanhao-wu.github.io/>`_
* Maya B. Gokhale

License
-------

|SCCL| follows the same licensing as clang. Please look at `LICENSE <LICENSE>`_.
