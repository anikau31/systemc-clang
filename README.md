[systemc-clang](http://github.com/anikau31/systemc-clang): SystemC Parser using the Clang Front-end
================================================================================================

[systemc-clang](https://github.com/anikau31/systemc-clang) is a revamp of an earlier [SystemC](http://systemc.org) Parser called [SystemCXML](http://systemcxml.sourceforge.net/).  The purpose of this project is to provide researchers and individuals with a parser for SystemC. This SystemC parser uses the [clang](http://clang.llvm.org) front-end. 

The public release will only allow parsing and representation of structural information of SystemC models at the RTL.  This implementation will provide a reflection data structure that a designer can use to extract this information. 

Internally, we are developing methods to represent behavioural information of the SystemC models.  In addition, we plan to support TLM 2.0 models as well.  Stay tuned for more updates and releases. 

As always, please feel free to provide issues, and feature requests.

Requirements
------------



* [llvm](http://llvm.org) 
* [clang](http://clang.llvm.org) (version 3.4)
* [SystemC](http://systemc.org) version 2.3
* Please see [SystemC Installation notes](https://github.com/anikau31/systemc-clang/blob/master/doc/systemc-install.mkd)

Installation
------------
Please see the instruction [here](https://github.com/anikau31/systemc-clang/blob/master/INSTALL.mkd)

Plugging into systemc-clang
---------------------------
Write a simple [plugin](https://github.com/anikau31/systemc-clang/blob/master/doc/plugins.mkd) to access the parsed information. 

Contact
-------
* [Anirudh M. Kaushik](https://ece.uwaterloo.ca/~anikau31/uwhtml/team/anirudh-kaushik/)

License
--------
* systemc-clang follows the same licensing as clang.  Please look at [LICENSE]( https://github.com/anikau31/systemc-clang/master/master/LICENSE.mkd).

Other Tools
------------

There are several other SystemC parsers that are available.  If you try them, and have some feedback for us, please let us know. 
* [SystemCXML:](http://systemcxml.sourceforge.net/)
* [Pinapa: A SystemC Front-end](http://greensocs.sourceforge.net/pinapa/)
* [KaSCPar: Karlsruhe SystemC Parser Suite](http://www.fzi.de/index.php/de/component/content/article/238-ispe-sim/4350-sim-tools-kascpar-examples)
