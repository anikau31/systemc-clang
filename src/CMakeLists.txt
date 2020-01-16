if (USE_SAUTO)
  add_library (libsystemc-clang 
  Automata.cpp
  SuspensionAutomata.cpp
  SCuitable/GlobalSuspensionAutomata.cpp
  SCuitable/FindGPUMacro.cpp
    )
endif (USE_SAUTO)

add_library (libsystemc-clang
  ModuleDecl.cpp
  FindModule.cpp
  # Matchers -- FindSCModules.cpp
  FindSCMain.cpp
  FindPorts.cpp
  FindTLMInterfaces.cpp
  FindEvents.cpp
  FindGlobalEvents.cpp
  FindEntryFunctions.cpp
  FindSensitivity.cpp
  FindWait.cpp
  FindNotify.cpp
  FindSignals.cpp
  FindSimTime.cpp
  FindTemplateTypes.cpp
  FindConstructor.cpp
  SystemCClang.cpp
  FindNetlist.cpp
  FindArgument.cpp
  FindTemplateParameters.cpp
  FindModuleInstance.cpp
  #  Automata.cpp
  #  SuspensionAutomata.cpp
  #  SCuitable/GlobalSuspensionAutomata.cpp
  #  SCuitable/FindGPUMacro.cpp
  #####################################
#From here the files are for the reflection database
####################################
  #Utility.cpp
  EntryFunctionContainer.cpp
  WaitContainer.cpp
  NotifyContainer.cpp
  EventDecl.cpp
  ProcessDecl.cpp
  Signal.cpp
  PortDecl.cpp
  InterfaceDecl.cpp
  Model.cpp
  WaitCalls.cpp
  NotifyCalls.cpp
  EventContainer.cpp

  )
