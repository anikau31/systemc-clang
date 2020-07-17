Match #1:

/home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:3:1: note: "root" binds here
SC_MODULE(counter) {
^~~~~~~~~~~~~~~~~~~~
/home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5: note: expanded from macro 'SC_MODULE'
    struct user_module_name : ::sc_core::sc_module
    ^

Match #2:

/home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:3:1: note: "root" binds here
SC_MODULE(counter) {
^~~~~~~~~~~~~~~~~~
/home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5: note: expanded from macro 'SC_MODULE'
    struct user_module_name : ::sc_core::sc_module
    ^~~~~~~~~~~~~~~~~~~~~~~

Match #3:

/home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:26:1: note: "root" binds here
SC_MODULE(DUT) {
^~~~~~~~~~~~~~~~
/home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5: note: expanded from macro 'SC_MODULE'
    struct user_module_name : ::sc_core::sc_module
    ^

Match #4:

/home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:26:1: note: "root" binds here
SC_MODULE(DUT) {
^~~~~~~~~~~~~~
/home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5: note: expanded from macro 'SC_MODULE'
    struct user_module_name : ::sc_core::sc_module
    ^~~~~~~~~~~~~~~~~~~~~~~
4 matches.


