add_library (libsystemc-clang
	ModuleDecl.cpp
	FindModule.cpp
	SCModules.cpp
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
	FindConstructor.cpp	
	SystemCClang.cpp
	FindNetlist.cpp
	FindArgument.cpp
    Automata.cpp 
    SuspensionAutomata.cpp
#	SCuitable/GlobalSuspensionAutomata.cpp	
#	SCuitable/FindGPUMacro.cpp	
####################################
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
    # AST MATCHERS
    # matchers/sc_module.cpp
    
	)
