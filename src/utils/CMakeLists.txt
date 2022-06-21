set( SCC_UTILS
  #APIntUtils.cpp
  ArrayTypeUtils.cpp
  CXXRecordDeclUtils.cpp
  CallExprUtils.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( utils ${SCC_UTILS})
