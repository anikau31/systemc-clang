Writing matchers with clang-query
==================================

`clang-query` is a clang tool that allows one to write AST matchers and test them. 
Using `clang-query` to write matchers to identify SystemC constructs is quite beneficial.

.. warning:: 
   There are some differences in what `clang-query` accepts, and what the clang tool actually accepts. 
   As a result, not all matchers that work in `clang-query` will compile in the clang tool. 
   Nonetheless, `clang-query` is an excellent way to start experimenting with writing a matcher.

Pre-requisites 
--------------

Ensure that the following environment variables have been set.
   * `SYSTEMC`

The `systemc-clang` distribution provides a script `scripts/run-clang-query.sh` that runs `clang-query` with the appropriate arguments to recognize SystemC models. 
If there is a need to add additional paths, it can be done the same way. 

An example: Matching SystemC module declarations
-------------------------------------------------

The best way to illustrate the use of `clang-query` is with examples. 
Suppose we have the following SystemC module declaration.

.. literalinclude:: matchers/counter.cpp

Without a doubt, anyone who wishes to write an AST matcher must review the list of matchers that are available here: `AST Matchers Reference <https://clang.llvm.org/docs/LibASTMatchersReference.html>`_. 

To run this example, we can execute it in the following way (assuming we are in the root of the `systemc-clang` directory).

.. code-block:: bash
   $ clang-query.sh -extra-arg=-I$SYSTEMC/include docs/source/matcher/counter.cpp

The `-extra-arg` option in `clang-query` specified the path for SystemC includes. 
You will come to a `clang-query` prompt.
This is where we can write our matcher.

Let us start by identifying the declaration of the SystemC module called `counter`.
In order to do this, we need to understand that the `SC_MODULE()` macro essentially declares a class that is derived from the `sc_module` class.
As a starter, we could write the following matcher.

.. code-block:: c++
   :linenos:
   match cxxRecordDecl()

Note that the command `match` tells that the string following it is the matcher to execute.
When you run the above command, you will notice that this produces quite a few matches (around nine thousand of them). 
This is because the matcher is identifying all C++ declarations in the AST, which includes the SystemC library. 

If we want to limit ourselves to only matching the file provided, then we can use the `isExpansionInMainFile()`. Please refer to the AST matcher reference to learn more about it. 

.. code-block:: c++
   :linenos:
   match cxxRecordDecl(isExpansionInMainFile())

This matcher will produce four matches of the two SystemC modules in the model.

.. code-block:: bash
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


You will quickly note that `clang-query` doesn't really provide a nice interface to go back to the previous command and edit it. 
Consequently, it is better to use a file to provide as an input to it with the matcher we wish to write.

Suppose that we create a separate file called `control.dbg`, which contains our matcher. 
We can then execute the script in the following way.
.. code-block:: bash
   $ clang-query.sh -extra-arg=-I$SYSTEMC/include docs/source/matcher/counter.cpp -f control.dbg

You will notice that we have multiple matches (more then 2), and we should only be having two matches for the two SystemC modules. 
To correct this, we have to make two changes:
* First is to inform `clang-query` that we should only receive matches on nodes that we have bound a string to, and disable outputting any other matches that are not specific to the strings the matcher binds. We would do this by using `set bind-root false`.
* Second, we have to remove implicit nodes that are created in the clang AST. 

If we want to write a matcher that refers to the identified AST nodes, we have to `bind` the nodes to a string that we can then use to extract them. We can update our matcher to add a binding string.
.. code-block:: c++
   :linenos:
   match cxxRecordDecl(unless(isImplicit()), isExpansionInMainFile()).bind("modules")

.. note:: 
   You may have noticed that we are finding the SystemC module declarations twice. 
   The reason for this is that clang creates an implicit referenced node for each of the classes.
   To remove this from our matches, we need to include the unless(isImplicit()) predicate to the matcher.

This will produce two matches, which correctly identify the `counter` and `DUT` SystemC modules.
If you wish to display the AST nodes for these matches, then you can add a command to control the output. This would be done with `set output dump`:

.. code-block:: c++
   :linenos:
   set bind-root false
   set output dump 
   match cxxRecordDecl(unless(isImplicit()), isExpansionInMainFile()).bind("modules")

.. code-block:: bash
Match #1:

Binding for "modules":
CXXRecordDecl 0x584e7c8 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:23:1> line:3:11 referenced struct counter definition
|-DefinitionData polymorphic has_user_declared_ctor has_mutable_fields can_const_default_init
| |-DefaultConstructor
| |-CopyConstructor non_trivial has_const_param needs_overload_resolution implicit_has_const_param
| |-MoveConstructor exists non_trivial needs_overload_resolution
| |-CopyAssignment non_trivial has_const_param needs_overload_resolution implicit_has_const_param
| |-MoveAssignment exists non_trivial needs_overload_resolution
| `-Destructor simple non_trivial needs_overload_resolution
|-public '::sc_core::sc_module':'sc_core::sc_module'
|-CXXRecordDecl 0x584e9a0 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:3:11> col:11 implicit referenced struct counter
|-FieldDecl 0x584ea60 <line:5:3, col:13> col:13 referenced clk 'sc_core::sc_in_clk':'sc_core::sc_in<bool>'
|-FieldDecl 0x5886a48 <line:8:3, col:23> col:23 referenced count_out 'sc_out<sc_uint<32> >':'sc_core::sc_out<sc_dt::sc_uint<32> >'
|-FieldDecl 0x588d7d0 <line:11:3, col:15> col:15 referenced keep_count 'sc_uint<32>':'sc_dt::sc_uint<32>'
|-TypedefDecl 0x588d830 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:400:5, col:30> col:30 referenced SC_CURRENT_USER_MODULE 'counter'
| `-RecordType 0x584e860 'counter'
|   `-CXXRecord 0x584e7c8 'counter'
|-CXXConstructorDecl 0x588da18 </home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:13:11, line:16:3> line:13:11 used counter 'void (::sc_core::sc_module_name)'
| |-ParmVarDecl 0x588d910 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:401:23, col:34> col:49 '::sc_core::sc_module_name':'sc_core::sc_module_name' destroyed
| |-CXXCtorInitializer '::sc_core::sc_module':'sc_core::sc_module'
| | `-CXXConstructExpr 0x588e310 </home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:13:11> '::sc_core::sc_module':'sc_core::sc_module' 'void ()'
| |-CXXCtorInitializer Field 0x584ea60 'clk' 'sc_core::sc_in_clk':'sc_core::sc_in<bool>'
| | `-CXXConstructExpr 0x588e390 <col:11> 'sc_core::sc_in_clk':'sc_core::sc_in<bool>' 'void ()'
| |-CXXCtorInitializer Field 0x5886a48 'count_out' 'sc_out<sc_uint<32> >':'sc_core::sc_out<sc_dt::sc_uint<32> >'
| | `-CXXConstructExpr 0x588e3e0 <col:11> 'sc_out<sc_uint<32> >':'sc_core::sc_out<sc_dt::sc_uint<32> >' 'void ()'
| |-CXXCtorInitializer Field 0x588d7d0 'keep_count' 'sc_uint<32>':'sc_dt::sc_uint<32>'
| | `-CXXConstructExpr 0x588e430 <col:11> 'sc_uint<32>':'sc_dt::sc_uint<32>' 'void ()'
| `-CompoundStmt 0x588f2c8 <col:20, line:16:3>
|   |-CompoundStmt 0x588f060 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:417:5, line:425:5>
|   | |-DeclStmt 0x588e9b8 <line:418:9, line:421:12>
|   | | `-VarDecl 0x588e530 <line:418:9, line:421:11> <scratch space>:76:1 used count_up_handle '::sc_core::sc_process_handle':'sc_core::sc_process_handle' cinit destroyed
|   | |   `-ExprWithCleanups 0x588e9a0 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:419:6, line:421:11> '::sc_core::sc_process_handle':'sc_core::sc_process_handle'
|   | |     `-CXXConstructExpr 0x588e970 <line:419:6, line:421:11> '::sc_core::sc_process_handle':'sc_core::sc_process_handle' 'void (const sc_core::sc_process_handle &)' elidable
|   | |       `-MaterializeTemporaryExpr 0x588e958 <line:419:6, line:421:11> 'const sc_core::sc_process_handle' lvalue
|   | |         `-ImplicitCastExpr 0x588e940 <line:419:6, line:421:11> 'const sc_core::sc_process_handle' <NoOp>
|   | |           `-CXXBindTemporaryExpr 0x588e920 <line:419:6, line:421:11> 'sc_core::sc_process_handle' (CXXTemporary 0x588e920)
|   | |             `-CXXMemberCallExpr 0x588e878 <line:419:6, line:421:11> 'sc_core::sc_process_handle'
|   | |               |-MemberExpr 0x588e638 <line:419:6, col:41> '<bound member function type>' ->create_method_process 0x4c4cbf8
|   | |               | `-CallExpr 0x588e618 <col:6, col:38> 'sc_core::sc_simcontext *'
|   | |               |   `-ImplicitCastExpr 0x588e600 <col:6, col:15> 'sc_core::sc_simcontext *(*)()' <FunctionToPointerDecay>
|   | |               |     `-DeclRefExpr 0x588e5d0 <col:6, col:15> 'sc_core::sc_simcontext *()' lvalue Function 0x4caae20 'sc_get_curr_simcontext' 'sc_core::sc_simcontext *()'
|   | |               |-ImplicitCastExpr 0x588e8c0 <<scratch space>:75:1> 'const char *' <ArrayToPointerDecay>
|   | |               | `-StringLiteral 0x588e668 <col:1> 'const char [9]' lvalue "count_up"
|   | |               |-CXXBoolLiteralExpr 0x588e688 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:420:10> 'bool' false
|   | |               |-CXXStaticCastExpr 0x588e808 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_process.h:151:9, col:64> 'sc_core::SC_ENTRY_FUNC':'void (sc_core::sc_process_host::*)()' static_cast<sc_core::SC_ENTRY_FUNC> <DerivedToBaseMemberPointer (sc_module -> sc_process_host)>
|   | |               | `-UnaryOperator 0x588e7d0 <col:45, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:14:15> 'void (counter::*)()' prefix '&' cannot overflow
|   | |               |   `-DeclRefExpr 0x588e770 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:457:29, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:14:15> 'void ()' CXXMethod 0x588dae8 'count_up' 'void ()'
|   | |               |-ImplicitCastExpr 0x588e8d8 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:421:3> 'sc_core::sc_process_host *' <DerivedToBase (sc_module -> sc_process_host)>
|   | |               | `-CXXThisExpr 0x588e848 <col:3> 'counter *' this
|   | |               `-ImplicitCastExpr 0x588e900 <col:9> 'const sc_core::sc_spawn_options *' <NullToPointer>
|   | |                 `-IntegerLiteral 0x588e858 <col:9> 'int' 0
|   | |-ExprWithCleanups 0x588ebe8 <line:422:9, <scratch space>:76:1> 'sc_core::sc_sensitive' lvalue
|   | | `-CXXOperatorCallExpr 0x588ebb0 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:422:9, <scratch space>:76:1> 'sc_core::sc_sensitive' lvalue
|   | |   |-ImplicitCastExpr 0x588eb98 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:422:25> 'sc_core::sc_sensitive &(*)(sc_core::sc_process_handle)' <FunctionToPointerDecay>
|   | |   | `-DeclRefExpr 0x588eb50 <col:25> 'sc_core::sc_sensitive &(sc_core::sc_process_handle)' lvalue CXXMethod 0x4af38d8 'operator<<' 'sc_core::sc_sensitive &(sc_core::sc_process_handle)'
|   | |   |-MemberExpr 0x588ea00 <col:9, col:15> 'sc_core::sc_sensitive' lvalue ->sensitive 0x4d52f98
|   | |   | `-ImplicitCastExpr 0x588e9e0 <col:9> 'sc_core::sc_module *' <UncheckedDerivedToBase (sc_module)>
|   | |   |   `-CXXThisExpr 0x588e9d0 <col:9> 'counter *' this
|   | |   `-CXXBindTemporaryExpr 0x588eb30 <<scratch space>:76:1> 'sc_core::sc_process_handle' (CXXTemporary 0x588eb30)
|   | |     `-CXXConstructExpr 0x588eaf8 <col:1> 'sc_core::sc_process_handle' 'void (const sc_core::sc_process_handle &)'
|   | |       `-ImplicitCastExpr 0x588eae0 <col:1> 'const sc_core::sc_process_handle' lvalue <NoOp>
|   | |         `-DeclRefExpr 0x588ea30 <col:1> '::sc_core::sc_process_handle':'sc_core::sc_process_handle' lvalue Var 0x588e530 'count_up_handle' '::sc_core::sc_process_handle':'sc_core::sc_process_handle'
|   | |-ExprWithCleanups 0x588ee18 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:423:9, <scratch space>:76:1> 'sc_core::sc_sensitive_pos' lvalue
|   | | `-CXXOperatorCallExpr 0x588ede0 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:423:9, <scratch space>:76:1> 'sc_core::sc_sensitive_pos' lvalue
|   | |   |-ImplicitCastExpr 0x588edc8 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:423:29> 'sc_core::sc_sensitive_pos &(*)(sc_core::sc_process_handle)' <FunctionToPointerDecay>
|   | |   | `-DeclRefExpr 0x588ed80 <col:29> 'sc_core::sc_sensitive_pos &(sc_core::sc_process_handle)' lvalue CXXMethod 0x4af7638 'operator<<' 'sc_core::sc_sensitive_pos &(sc_core::sc_process_handle)'
|   | |   |-MemberExpr 0x588ec30 <col:9, col:15> 'sc_core::sc_sensitive_pos' lvalue ->sensitive_pos 0x4d52ff8
|   | |   | `-ImplicitCastExpr 0x588ec10 <col:9> 'sc_core::sc_module *' <UncheckedDerivedToBase (sc_module)>
|   | |   |   `-CXXThisExpr 0x588ec00 <col:9> 'counter *' this
|   | |   `-CXXBindTemporaryExpr 0x588ed60 <<scratch space>:76:1> 'sc_core::sc_process_handle' (CXXTemporary 0x588ed60)
|   | |     `-CXXConstructExpr 0x588ed28 <col:1> 'sc_core::sc_process_handle' 'void (const sc_core::sc_process_handle &)'
|   | |       `-ImplicitCastExpr 0x588ed10 <col:1> 'const sc_core::sc_process_handle' lvalue <NoOp>
|   | |         `-DeclRefExpr 0x588ec60 <col:1> '::sc_core::sc_process_handle':'sc_core::sc_process_handle' lvalue Var 0x588e530 'count_up_handle' '::sc_core::sc_process_handle':'sc_core::sc_process_handle'
|   | `-ExprWithCleanups 0x588f048 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:424:9, <scratch space>:76:1> 'sc_core::sc_sensitive_neg' lvalue
|   |   `-CXXOperatorCallExpr 0x588f010 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:424:9, <scratch space>:76:1> 'sc_core::sc_sensitive_neg' lvalue
|   |     |-ImplicitCastExpr 0x588eff8 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:424:29> 'sc_core::sc_sensitive_neg &(*)(sc_core::sc_process_handle)' <FunctionToPointerDecay>
|   |     | `-DeclRefExpr 0x588efb0 <col:29> 'sc_core::sc_sensitive_neg &(sc_core::sc_process_handle)' lvalue CXXMethod 0x4afa2e8 'operator<<' 'sc_core::sc_sensitive_neg &(sc_core::sc_process_handle)'
|   |     |-MemberExpr 0x588ee60 <col:9, col:15> 'sc_core::sc_sensitive_neg' lvalue ->sensitive_neg 0x4d53058
|   |     | `-ImplicitCastExpr 0x588ee40 <col:9> 'sc_core::sc_module *' <UncheckedDerivedToBase (sc_module)>
|   |     |   `-CXXThisExpr 0x588ee30 <col:9> 'counter *' this
|   |     `-CXXBindTemporaryExpr 0x588ef90 <<scratch space>:76:1> 'sc_core::sc_process_handle' (CXXTemporary 0x588ef90)
|   |       `-CXXConstructExpr 0x588ef58 <col:1> 'sc_core::sc_process_handle' 'void (const sc_core::sc_process_handle &)'
|   |         `-ImplicitCastExpr 0x588ef40 <col:1> 'const sc_core::sc_process_handle' lvalue <NoOp>
|   |           `-DeclRefExpr 0x588ee90 <col:1> '::sc_core::sc_process_handle':'sc_core::sc_process_handle' lvalue Var 0x588e530 'count_up_handle' '::sc_core::sc_process_handle':'sc_core::sc_process_handle'
|   |-NullStmt 0x588f090 </home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:14:24>
|   `-CXXOperatorCallExpr 0x588f290 <line:15:5, col:26> 'sc_core::sc_sensitive' lvalue
|     |-ImplicitCastExpr 0x588f278 <col:15> 'sc_core::sc_sensitive &(*)(sc_core::sc_event_finder &)' <FunctionToPointerDecay>
|     | `-DeclRefExpr 0x588f230 <col:15> 'sc_core::sc_sensitive &(sc_core::sc_event_finder &)' lvalue CXXMethod 0x4af4438 'operator<<' 'sc_core::sc_sensitive &(sc_core::sc_event_finder &)'
|     |-MemberExpr 0x588f0c8 <col:5> 'sc_core::sc_sensitive' lvalue ->sensitive 0x4d52f98
|     | `-ImplicitCastExpr 0x588f0a8 <col:5> 'sc_core::sc_module *' <UncheckedDerivedToBase (sc_module)>
|     |   `-CXXThisExpr 0x588f098 <col:5> 'counter *' implicit this
|     `-CXXMemberCallExpr 0x588f168 <col:18, col:26> 'sc_core::sc_event_finder' lvalue
|       `-MemberExpr 0x588f138 <col:18, col:22> '<bound member function type>' .pos 0x4f24958
|         `-ImplicitCastExpr 0x588f188 <col:18> 'const sc_core::sc_in<bool>' lvalue <NoOp>
|           `-MemberExpr 0x588f108 <col:18> 'sc_core::sc_in_clk':'sc_core::sc_in<bool>' lvalue ->clk 0x584ea60
|             `-CXXThisExpr 0x588f0f8 <col:18> 'counter *' implicit this
|-CXXMethodDecl 0x588dae8 <line:18:3, line:21:3> line:18:8 used count_up 'void ()'
| `-CompoundStmt 0x588f720 <col:19, line:21:3>
|   |-CXXOperatorCallExpr 0x588f5b0 <line:19:5, col:31> 'sc_uint<32>':'sc_dt::sc_uint<32>' lvalue
|   | |-ImplicitCastExpr 0x588f598 <col:16> 'sc_uint<32> &(*)(sc_dt::uint_type)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x588f518 <col:16> 'sc_uint<32> &(sc_dt::uint_type)' lvalue CXXMethod 0x5888a60 'operator=' 'sc_uint<32> &(sc_dt::uint_type)'
|   | |-MemberExpr 0x588f300 <col:5> 'sc_uint<32>':'sc_dt::sc_uint<32>' lvalue ->keep_count 0x588d7d0
|   | | `-CXXThisExpr 0x588f2f0 <col:5> 'counter *' implicit this
|   | `-BinaryOperator 0x588f438 <col:18, col:31> 'unsigned long long' '+'
|   |   |-ImplicitCastExpr 0x588f408 <col:18> 'sc_dt::uint_type':'unsigned long long' <UserDefinedConversion>
|   |   | `-CXXMemberCallExpr 0x588f3e8 <col:18> 'sc_dt::uint_type':'unsigned long long'
|   |   |   `-MemberExpr 0x588f3b8 <col:18> '<bound member function type>' .operator unsigned long long 0x5259170
|   |   |     `-ImplicitCastExpr 0x588f398 <col:18> 'const sc_dt::sc_uint_base' lvalue <UncheckedDerivedToBase (sc_uint_base)>
|   |   |       `-MemberExpr 0x588f340 <col:18> 'sc_uint<32>':'sc_dt::sc_uint<32>' lvalue ->keep_count 0x588d7d0
|   |   |         `-CXXThisExpr 0x588f330 <col:18> 'counter *' implicit this
|   |   `-ImplicitCastExpr 0x588f420 <col:31> 'unsigned long long' <IntegralCast>
|   |     `-IntegerLiteral 0x588f370 <col:31> 'int' 1
|   `-CXXMemberCallExpr 0x588f698 <line:20:5, col:33> 'void'
|     |-MemberExpr 0x588f628 <col:5, col:15> '<bound member function type>' .write 0x5882db8
|     | `-ImplicitCastExpr 0x588f6e8 <col:5> 'sc_core::sc_inout<sc_dt::sc_uint<32> >' lvalue <UncheckedDerivedToBase (sc_inout)>
|     |   `-MemberExpr 0x588f5f8 <col:5> 'sc_out<sc_uint<32> >':'sc_core::sc_out<sc_dt::sc_uint<32> >' lvalue ->count_out 0x5886a48
|     |     `-CXXThisExpr 0x588f5e8 <col:5> 'counter *' implicit this
|     `-ImplicitCastExpr 0x588f708 <col:22> 'const sc_core::sc_inout<sc_dt::sc_uint<32> >::data_type':'const sc_dt::sc_uint<32>' lvalue <NoOp>
|       `-MemberExpr 0x588f668 <col:22> 'sc_uint<32>':'sc_dt::sc_uint<32>' lvalue ->keep_count 0x588d7d0
|         `-CXXThisExpr 0x588f658 <col:22> 'counter *' implicit this
|-CXXConstructorDecl 0x588dbb8 <line:3:11> col:11 implicit counter 'void (const counter &)' inline default_delete noexcept-unevaluated 0x588dbb8
| `-ParmVarDecl 0x588dcc8 <col:11> col:11 'const counter &'
|-CXXConstructorDecl 0x588dd68 <col:11> col:11 implicit counter 'void (counter &&)' inline default_delete noexcept-unevaluated 0x588dd68
| `-ParmVarDecl 0x588de78 <col:11> col:11 'counter &&'
|-CXXMethodDecl 0x588df18 <col:11> col:11 implicit operator= 'counter &(const counter &)' inline default_delete noexcept-unevaluated 0x588df18
| `-ParmVarDecl 0x588e028 <col:11> col:11 'const counter &'
|-CXXMethodDecl 0x588e098 <col:11> col:11 implicit operator= 'counter &(counter &&)' inline default_delete noexcept-unevaluated 0x588e098
| `-ParmVarDecl 0x588e1a8 <col:11> col:11 'counter &&'
`-CXXDestructorDecl 0x588e230 <col:11> col:11 implicit used ~counter 'void () throw()' inline default
  |-Overrides: [ 0x4d4cfc8 sc_module::~sc_module 'void () throw()' ]
  `-CompoundStmt 0x5896c80 <col:11>


Match #2:

Binding for "modules":
CXXRecordDecl 0x588f740 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:36:1> line:26:11 referenced struct DUT definition
|-DefinitionData polymorphic has_user_declared_ctor has_mutable_fields can_const_default_init
| |-DefaultConstructor
| |-CopyConstructor non_trivial has_const_param needs_overload_resolution implicit_has_const_param
| |-MoveConstructor exists non_trivial needs_overload_resolution
| |-CopyAssignment non_trivial has_const_param needs_overload_resolution implicit_has_const_param
| |-MoveAssignment exists non_trivial needs_overload_resolution
| `-Destructor simple non_trivial needs_overload_resolution
|-public '::sc_core::sc_module':'sc_core::sc_module'
|-CXXRecordDecl 0x588f8d8 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:397:5, /home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:26:11> col:11 implicit referenced struct DUT
|-FieldDecl 0x588f978 <line:27:3, col:11> col:11 referenced count 'counter'
|-FieldDecl 0x588f9d8 <line:28:3, col:12> col:12 referenced clock 'sc_core::sc_clock'
|-FieldDecl 0x5895f38 <line:29:3, col:28> col:28 referenced counter_out 'sc_signal<sc_uint<32> >':'sc_core::sc_signal<sc_dt::sc_uint<32>, sc_core::SC_ONE_WRITER>'
|-TypedefDecl 0x5895f98 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:400:5, col:30> col:30 SC_CURRENT_USER_MODULE 'DUT'
| `-RecordType 0x588f7d0 'DUT'
|   `-CXXRecord 0x588f740 'DUT'
|-CXXConstructorDecl 0x5896118 </home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:31:11, line:35:3> line:31:11 used DUT 'void (::sc_core::sc_module_name)'
| |-ParmVarDecl 0x5896040 </home/twiga/code/systemc-2.3.3/systemc/include/sysc/kernel/sc_module.h:401:23, col:34> col:49 '::sc_core::sc_module_name':'sc_core::sc_module_name' destroyed
| |-CXXCtorInitializer '::sc_core::sc_module':'sc_core::sc_module'
| | `-CXXConstructExpr 0x5896b40 </home/twiga/code/github/systemc-clang/docs/source/developer/matchers/counter.cpp:31:11> '::sc_core::sc_module':'sc_core::sc_module' 'void ()'
| |-CXXCtorInitializer Field 0x588f978 'count' 'counter'
| | `-ExprWithCleanups 0x5896b00 <col:17, col:41> 'counter'
| |   `-CXXConstructExpr 0x5896ad0 <col:17, col:41> 'counter' 'void (::sc_core::sc_module_name)' list
| |     `-CXXBindTemporaryExpr 0x5896ab0 <col:23> '::sc_core::sc_module_name':'sc_core::sc_module_name' (CXXTemporary 0x5896ab0)
| |       `-CXXConstructExpr 0x5896a78 <col:23> '::sc_core::sc_module_name':'sc_core::sc_module_name' 'void (const sc_core::sc_module_name &)' elidable
| |         `-MaterializeTemporaryExpr 0x5896a60 <col:23> 'const ::sc_core::sc_module_name':'const sc_core::sc_module_name' lvalue
| |           `-ImplicitCastExpr 0x5896a48 <col:23> 'const ::sc_core::sc_module_name':'const sc_core::sc_module_name' <NoOp>
| |             `-CXXBindTemporaryExpr 0x5896a28 <col:23> '::sc_core::sc_module_name':'sc_core::sc_module_name' (CXXTemporary 0x5896a28)
| |               `-ImplicitCastExpr 0x5896a08 <col:23> '::sc_core::sc_module_name':'sc_core::sc_module_name' <ConstructorConversion>
| |                 `-CXXConstructExpr 0x58969d8 <col:23> '::sc_core::sc_module_name':'sc_core::sc_module_name' 'void (const char *)'
| |                   `-ImplicitCastExpr 0x58969c0 <col:23> 'const char *' <ArrayToPointerDecay>
| |                     `-StringLiteral 0x5896950 <col:23> 'const char [17]' lvalue "counter_instance"
| |-CXXCtorInitializer Field 0x588f9d8 'clock' 'sc_core::sc_clock'
| | `-CXXConstructExpr 0x5896bc0 <col:11> 'sc_core::sc_clock' 'void ()'
| |-CXXCtorInitializer Field 0x5895f38 'counter_out' 'sc_signal<sc_uint<32> >':'sc_core::sc_signal<sc_dt::sc_uint<32>, sc_core::SC_ONE_WRITER>'
| | `-CXXConstructExpr 0x5896c10 <col:11> 'sc_signal<sc_uint<32> >':'sc_core::sc_signal<sc_dt::sc_uint<32>, sc_core::SC_ONE_WRITER>' 'void ()'
| `-CompoundStmt 0x58970f8 <col:43, line:35:3>
|   |-CXXOperatorCallExpr 0x5896e60 <line:33:5, col:20> 'void'
|   | |-ImplicitCastExpr 0x5896df8 <col:14, col:20> 'void (*)(const sc_core::sc_in<bool>::in_if_type &)' <FunctionToPointerDecay>
|   | | `-DeclRefExpr 0x5896d78 <col:14, col:20> 'void (const sc_core::sc_in<bool>::in_if_type &)' lvalue CXXMethod 0x4f23e40 'operator()' 'void (const sc_core::sc_in<bool>::in_if_type &)'
|   | |-MemberExpr 0x5896cd0 <col:5, col:11> 'sc_core::sc_in_clk':'sc_core::sc_in<bool>' lvalue .clk 0x584ea60
|   | | `-MemberExpr 0x5896ca0 <col:5> 'counter' lvalue ->count 0x588f978
|   | |   `-CXXThisExpr 0x5896c90 <col:5> 'DUT *' implicit this
|   | `-ImplicitCastExpr 0x5896e28 <col:15> 'const sc_core::sc_in<bool>::in_if_type':'const sc_core::sc_signal_in_if<bool>' lvalue <DerivedToBase (sc_signal -> sc_signal_t -> sc_signal_inout_if -> sc_signal_in_if)>
|   |   `-ImplicitCastExpr 0x5896e10 <col:15> 'const sc_core::sc_clock' lvalue <NoOp>
|   |     `-MemberExpr 0x5896d10 <col:15> 'sc_core::sc_clock' lvalue ->clock 0x588f9d8
|   |       `-CXXThisExpr 0x5896d00 <col:15> 'DUT *' implicit this
|   `-CXXOperatorCallExpr 0x58970c0 <line:34:5, col:32> 'void'
|     |-ImplicitCastExpr 0x5897028 <col:20, col:32> 'void (*)(sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> > &)' <FunctionToPointerDecay>
|     | `-DeclRefExpr 0x5896fb0 <col:20, col:32> 'void (sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> > &)' lvalue CXXMethod 0x584fdb0 'operator()' 'void (sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> > &)'
|     |-ImplicitCastExpr 0x5897068 <col:5, col:11> 'sc_core::sc_port_b<sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> > >' lvalue <UncheckedDerivedToBase (sc_inout -> sc_port -> sc_port_b)>
|     | `-MemberExpr 0x5896ed8 <col:5, col:11> 'sc_out<sc_uint<32> >':'sc_core::sc_out<sc_dt::sc_uint<32> >' lvalue .count_out 0x5886a48
|     |   `-MemberExpr 0x5896ea8 <col:5> 'counter' lvalue ->count 0x588f978
|     |     `-CXXThisExpr 0x5896e98 <col:5> 'DUT *' implicit this
|     `-ImplicitCastExpr 0x5897098 <col:21> 'sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> >':'sc_core::sc_signal_inout_if<sc_dt::sc_uint<32> >' lvalue <DerivedToBase (sc_signal_t -> sc_signal_inout_if)>
|       `-MemberExpr 0x5896f18 <col:21> 'sc_signal<sc_uint<32> >':'sc_core::sc_signal<sc_dt::sc_uint<32>, sc_core::SC_ONE_WRITER>' lvalue ->counter_out 0x5895f38
|         `-CXXThisExpr 0x5896f08 <col:21> 'DUT *' implicit this
|-CXXConstructorDecl 0x58961f8 <line:26:11> col:11 implicit DUT 'void (const DUT &)' inline default_delete noexcept-unevaluated 0x58961f8
| `-ParmVarDecl 0x5896308 <col:11> col:11 'const DUT &'
|-CXXConstructorDecl 0x58963a8 <col:11> col:11 implicit DUT 'void (DUT &&)' inline default_delete noexcept-unevaluated 0x58963a8
| `-ParmVarDecl 0x58964b8 <col:11> col:11 'DUT &&'
|-CXXMethodDecl 0x5896558 <col:11> col:11 implicit operator= 'DUT &(const DUT &)' inline default_delete noexcept-unevaluated 0x5896558
| `-ParmVarDecl 0x5896668 <col:11> col:11 'const DUT &'
|-CXXMethodDecl 0x58966d8 <col:11> col:11 implicit operator= 'DUT &(DUT &&)' inline default_delete noexcept-unevaluated 0x58966d8
| `-ParmVarDecl 0x58967e8 <col:11> col:11 'DUT &&'
`-CXXDestructorDecl 0x5896870 <col:11> col:11 implicit used ~DUT 'void () throw()' inline default
  |-Overrides: [ 0x4d4cfc8 sc_module::~sc_module 'void () throw()' ]
  `-CompoundStmt 0x5897428 <col:11>

2 matches.

 
