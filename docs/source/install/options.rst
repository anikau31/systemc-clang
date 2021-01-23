.. highlight:: console

Compile Options
---------------------------------

``systemc-clang`` enables certain features through compiler flag options.

+----------------------+-----------------+----------------------------------------------------+
| Option               | Arguments       | Description                                        |
+======================+=================+====================================================+
| ``SYSTEMC_DIR``      | <path>          | Path to SystemC installation.                      |
+----------------------+-----------------+----------------------------------------------------+
| ``LLVM_INSTALL_DIR`` | <path>          | Path to LLVM/Clang installation.                   |
+----------------------+-----------------+----------------------------------------------------+
| ``CMAKE_BUILD_TYPE`` | [Release,Debug] | Build type. Use Release unless developing.         |
+----------------------+-----------------+----------------------------------------------------+
| ``ENABLE_TESTS``     | [ON,OFF]        | Compiles tests.                                    |
+----------------------+-----------------+----------------------------------------------------+
| ``USE_CCACHE``       | [ON,OFF]        | Uses ccache for faster recompilation               |
+----------------------+-----------------+----------------------------------------------------+
| ``HDL``              | [ON,OFF]        | Compiles the translator plugin to generate Verilog |
+----------------------+-----------------+----------------------------------------------------+

These can be supplied to ``cmake`` via the ``-D`` option flag. 
