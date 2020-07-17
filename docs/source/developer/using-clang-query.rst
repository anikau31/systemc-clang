Writing matchers with `clang-query`
==================================

``clang-query`` is a clang tool that allows one to write AST matchers and test them. 
Using ``clang-query`` to write matchers to identify SystemC constructs is quite beneficial.

.. warning:: 
   There are some differences in what ``clang-query`` accepts, and what the clang tool actually accepts. 
   As a result, not all matchers that work in ``clang-query`` will compile in the clang tool. 
   Nonetheless, ``clang-query`` is an excellent way to start experimenting with writing a matcher.


Pre-requisites 
---------------

Ensure that the following environment variables have been set.
* ``SYSTEMC``


An example: Matching SystemC module declarations
-------------------------------------------------

The best way to illustrate the use of ``clang-query`` is with examples. 
Suppose we have the following SystemC module declaration.

.. literalinclude:: matchers/counter.cpp

Without a doubt, anyone who wishes to write an AST matcher must review the list of matchers that are available here: `AST Matchers Reference <https://clang.llvm.org/docs/LibASTMatchersReference.html>`_. 

To run this example, we can execute it in the following way (assuming we are in the root of the `systemc-clang` directory).

.. code-block:: c++
   $ clang-query -extra-arg=-I$SYSTEMC/include docs/source/matcher/counter.cpp

The ``-extra-arg`` option in ``clang-query`` specified the path for SystemC includes. 
You will come to a ``clang-query`` prompt.
This is where we can write our matcher.

Let us start by identifying the declarations of the SystemC modules in the model.
These would be the ``counter`` and ``DUT`` modules.
In order to do this, we need to understand that the ``SC_MODULE()`` macro essentially declares a class that is derived from the ``sc_module`` class.
As a starter, we could write the following matcher.

.. code-block:: c++
   :linenos:
   match cxxRecordDecl()

Note that the command ``match`` tells that the string following it is the matcher to execute.
When you run the above command, you will notice that this produces quite a few matches (around nine thousand of them). 
This is because the matcher is identifying all C++ declarations in the AST, which includes the SystemC library. 

If we want to limit ourselves to only matching the file provided, then we can use the ``isExpansionInMainFile()``. Please refer to the AST matcher reference to learn more about it. 

.. code-block:: c++
   :linenos:
   match cxxRecordDecl(isExpansionInMainFile())

This matcher will produce four matches of the two SystemC modules in the model.

.. literalinclude:: matchers/four-match.cpp

You will quickly note that ``clang-query`` doesn't really provide a nice interface to go back to the previous command and edit it. 
Consequently, it is better to use a file to provide as an input to it with the matcher we wish to write.

Suppose that we create a separate file called ``control.dbg``, which contains our matcher. 
We can then execute the script in the following way.

.. code-block:: c++ 
   $ `clang-query`.sh -extra-arg=-I$SYSTEMC/include docs/source/matcher/counter.cpp -f control.dbg

You will notice that we have multiple matches (more then 2), and we should only be having two matches for the two SystemC modules. 
To correct this, we have to make two changes:
* First is to inform ``clang-query`` that we should only receive matches on nodes that we have bound a string to, and disable outputting any other matches that are not specific to the strings the matcher binds. We would do this by using `set bind-root false`.
* Second, we have to remove implicit nodes that are created in the clang AST. 

If we want to write a matcher that refers to the identified AST nodes, we have to `bind` the nodes to a string that we can then use to extract them. We can update our matcher to add a binding string.

.. code-block:: c++
   :linenos:
   match cxxRecordDecl(unless(isImplicit()), isExpansionInMainFile()).bind("modules")

.. note:: 
   You may have noticed that we are finding the SystemC module declarations twice. 
   The reason for this is that clang creates an implicit referenced node for each of the classes.
   To remove this from our matches, we need to include the unless(isImplicit()) predicate to the matcher.

This will produce two matches, which correctly identify the ``counter`` and ``DUT`` SystemC modules.
If you wish to display the AST nodes for these matches, then you can add a command to control the output. This would be done with ``set output dump``:

.. code-block:: c++
   :linenos:
   set bind-root false
   set output dump 
   match cxxRecordDecl(unless(isImplicit()), isExpansionInMainFile()).bind("modules")

The output has not been shown, but hopefully you were able to see the AST dump.

Something that you may quickly notice that the matcher written would also identify classes that are not SystemC modules.
In order to enforce that, we need to state that the class must be derived from the ``sc_module`` class.
We would update our matcher in the following way.

.. code-block:: c++
   :linenos:
   set bind-root false
   set output dump 
   match cxxRecordDecl(isDerivedFrom("::sc_core::sc_module"), unless(isImplicit()), isExpansionInMainFile()).bind("modules")

If you were to go and add non-SystemC classes to the original model, you will see that those will not be matched.

