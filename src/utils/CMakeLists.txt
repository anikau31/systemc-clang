set( SCC_UTILS
  ArrayTypeUtils 
  CXXRecordDeclUtils
  )

include_directories(${SCC_INCLUDES})

add_library( utils ${SCC_UTILS})
