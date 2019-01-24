Writing a plugin for systemc-clang
==================================

These instructions will help in integrating your code with systemc-clang.  This will allow your code to interface with the data structure created by systemc-clang that holds the parsed information.

The steps involved in getting access to the data structure after systemc-clang has parsed the information are as follows.

* Define a class that inherits from `SystemCConsumer`.  Let us for our example, declare this class to be ModelDump.  Notice a few things here.  We are going to inherit from `SystemCConsumer` class, which is responsible for parsing and creating the data structure. The constructor passes a `CompilerInstance` reference to the base class.  This is necessary so for the systemc-clang to know parameters to the clang. 

```c++
#include "SystemCClang.h"

using namespace clang;
using namespace scpar;

class ModelDump
: public SystemCConsumer {

public:
  ModelDump( CompilerInstance& ci)
  : SystemCConsumer(ci) {
  
  }
  
  // Virtual methods.
  bool postFire() {
    Model* model = getSystemCModel();
    
    // Just dump the model traits.
    llvm::errs() << "\n Datastructure dump from static analysis\n";
    model->dump(llvm::errs());
    return true;
  } 
};
```

* `SystemCConsumer` provides a virtual method `postFire` that can be overridden. The overridden `postFire` method uses `getSystemCModel()` to retrieve a pointer to the data structure that stores the parsed information. 

* Invoking `model->dump(llvm::errs());` prints out the information stored in the data structure.  Additional methods can be found at [Model.h](https://github.com/anikau31/systemc-clang/blob/revamp/src/Model.h).

* Edit the [driver-tooling.cpp]() file with the following changes.
** Include your class header file.
** Provide a template argument that is the class name of the created class.  For our example, we will provide `ModelDump` as the class name.

```c++
#include "PluginAction.h"
using namespace scpar;

// Include your class
#include "ModelDump.h"

int main(int argc, const char **argv)
{
     PluginAction<ModelDump> pa(argc, argv);
     return 0;
}
```

* Compile systemc-clang.  The executable will now execute the method `postFire` after the SystemC file is parsed. 
