set( SCC_MODEL
  ModuleDecl.cpp
  EntryFunctionContainer.cpp
  WaitContainer.cpp
  NotifyContainer.cpp
  EventDecl.cpp
  ProcessDecl.cpp
  SignalDecl.cpp
  PortDecl.cpp
  InterfaceDecl.cpp
  Model.cpp
  WaitCalls.cpp
  NotifyCalls.cpp
  EventContainer.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( model ${SCC_MODEL})
