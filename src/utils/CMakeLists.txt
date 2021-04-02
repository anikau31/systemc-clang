set( SCC_UTILS
  ArrayTypeUtils.cpp
  CXXRecordDeclUtils.cpp
  )

include_directories(${SCC_INCLUDES})

add_library( utils ${SCC_UTILS})
