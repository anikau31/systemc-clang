
if (USE_SAUTO)
  add_library (libsystemc-clang 
  Automata.cpp
  SuspensionAutomata.cpp
  SCuitable/GlobalSuspensionAutomata.cpp
  SCuitable/FindGPUMacro.cpp
    )
endif (USE_SAUTO)


include_directories(${SCC_INCLUDE})

add_subdirectory( model )
add_subdirectory( matchers )
add_subdirectory( utils )

add_library(libsystemc-clang SystemCClang.cpp)

target_link_libraries(libsystemc-clang 
  model
  matchers
  utils
  )

