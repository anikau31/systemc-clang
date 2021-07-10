set( CFG_ANALYSIS 
  SplitCFG.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( cfg ${CFG_ANALYSIS})
