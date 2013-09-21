#include "ReflectionContainerClass.h"
#include "EntryFunctions.h"

EntryFunctions::EntryFunctions(string name, EntryFunctionType * entryType) : ReflectionContainerClass(name)  {
  recordInstance = entryType;
  type = ENTRYFUNCTION;
}

EntryFunctions::EntryFunctionType * EntryFunctions::getrecordInstance() {
  return recordInstance;
}

vector<ReflectionContainerClass*> * EntryFunctions::getSCModuleEntryChildren() {
  return getChildListSubset(ENTRYFUNCTION);
}
