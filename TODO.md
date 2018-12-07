Urgent
=======
- There is duplication between SignalInstanceContainer class and a Signal, and SignalContainer. This needs to be fixed to avoid further confusions.


In Progress
============
- Find connection netlist (bindings)
- Provide data structure for querying structural information

Fixed
======
- Find instances of sc_modules.
- Find sc_main function

Older
============
*Supported but refinements necessary*

FindPorts.h
-----------
- DONE: Default types are not handled, but template types are !! huh!

FindEntryFunctions.h
--------------------
- Multiple entry functions in different constructors of the same class.

FindSensitivity.h
--------------------
- We only handle "sensitive << a << b" type calls. 
- Have to support .pos(), .neg(), and sensitive_neg/sensitive_pos.


Compiling SystemC 2.2.0 Instructions
----------------------------------------

- On Mac OSX:
-- ../configure --build=i386-pc-linux (must pass the pc-linux build flag)
-- make[3]: Entering directory xxxx/systemc-2.2.0/src/sysc/qt'
as -o qtmds.o qtmds.s -I. -I. -I. -I../../../src
qtmds.s: Assembler messages:
qtmds.s:69: Error: suffix or operands invalid for `push'
qtmds.s:70: Error: suffix or operands invalid for `push
....

The solution is:
1)remove the file xxxx/systemc-2.2.0/src/sysc/qt/qtmds.s
2)go under xxxx/systemc-2.2.0/src/sysc/qt and execute this command:ln -s
md/iX86_64.s qtmds.s to get the "qtmds.s" back but with the correct
version (64 bits), then execute make under the
3)execute make
4) go back to xxxx/systemc-2.2.0 and execute make
