set (SCC_MATCHERS
  FindModule.cpp
  FindSCMain.cpp
  FindEvents.cpp
  FindGlobalEvents.cpp
  FindEntryFunctions.cpp
  FindSensitivity.cpp
  FindWait.cpp
  FindNotify.cpp
  FindSimTime.cpp
  FindTemplateTypes.cpp
  FindConstructor.cpp
  FindNetlist.cpp
  FindArgument.cpp
  FindTemplateParameters.cpp
  FindModuleInstance.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( matchers ${SCC_MATCHERS})
