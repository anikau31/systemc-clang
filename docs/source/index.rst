.. systemc-clang documentation master file, created by
   sphinx-quickstart on Sun Jun 14 14:49:35 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to systemc-clang's documentation!
=========================================

.. toctree::
  :maxdepth: 2
  :hidden:
  :caption: Contents

  install/install.rst
  usage.rst

  examples.rst

  support.rst

.. toctree::
  :maxdepth: 2
  :hidden:
  :caption: Developer

  developer/developer.rst

  plugins/xlat.rst

.. toctree::
  :maxdepth: 2
  :hidden:
  :caption: Code

  api/library_root


About
^^^^^^

``systemc-clang`` is a clang tool that parses SystemC 2.3.3 models. 
The tool parses RTL constructs and some TLM 2.0 constructs.
Recently, with a plugin to generate Verilog has been added.


Requirements
^^^^^^^^^^^^

``systemc-clang`` has the following requirements.
* `clang <http://clang.llvm.org/download.html>`_ version 10.0.0.
* `SystemC <http://systemc.org>`_ version 2.3.3. Please see `installation notes <https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd>`_
* c++14 is required.  We are using some features that necessitate c++14.  Down-porting it is also possible, but not supported.


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
