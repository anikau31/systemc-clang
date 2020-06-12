set (SCC_MATCHERS
  FindSCMain.cpp
  FindEvents.cpp
  FindGlobalEvents.cpp
  FindEntryFunctions.cpp
  FindWait.cpp
  FindNotify.cpp
  FindSimTime.cpp
  FindTemplateTypes.cpp
  FindConstructor.cpp
  FindArgument.cpp
  FindTemplateParameters.cpp
  FindNetlist.cpp
  #FindModule.cpp
  #FindSensitivity.cpp
  #FindModuleInstance.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( matchers ${SCC_MATCHERS})
