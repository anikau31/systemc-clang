set( CFG_ANALYSIS 
  SplitCFGBlock.cpp
  SplitCFG.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( cfg ${CFG_ANALYSIS})
