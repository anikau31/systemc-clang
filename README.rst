A SystemC Parser using the Clang Front-end
==========================================

`systemc-clang <https://git.uwaterloo.ca/caesr-pub//systemc-clang>`__
parses SystemC 2.3.3. It parses RTL constructs and some TLM 2.0
constructs. This is the maintained repository of an older version
available on `github <https://github.com/anikau31/systemc-clang>`__.

Requirements
------------

-  `llvm/clang <https://releases.llvm.org/download.html>`__\ (version
   7.0.0)
-  `SystemC <http://systemc.org>`__ version 2.3.3. Please see `SystemC
   Installation
   notes <https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd>`__
-  c++14 is required. We are using some features that necessitate c++14.
   Down-porting it is also possible, but not supported.

Installation
------------

-  `Linux <INSTALL-linux.md>`__
-  `OSX <INSTALL-osx.md>`__

Contact
-------

If you encounter problems, please create issues with a minimally working
example that illustrates the issue.

For other concerns and comments, please contact us directly. \* `Anirudh
M.
Kaushik <https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/>`__
\* `Hiren Patel <https://caesr.uwaterloo.ca>`__

License
-------

systemc-clang follows the same licensing as clang. Please look at
`LICENSE <https://git.uwaterloo.ca/caesr-pub/systemc-clang/LICENSE.md>`__.
