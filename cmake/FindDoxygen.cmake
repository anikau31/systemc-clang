
if (BUILD_DOC)
##################################################
# Sphinx
##################################################
include(FindSphinx)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs/source/)
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/)

add_custom_target(sphinx
                  COMMAND
                  ${SPHINX_EXECUTABLE} -M html
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")



##################################################
# Doxygen
##################################################
# look for Doxygen package
find_package(Doxygen)

if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/source//Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # Note: do not put "ALL" - this builds docs together with application EVERY TIME!
    add_custom_target( doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
  message(FATAL_ERROR "FATAL: DOXYGEN not found.")

endif (DOXYGEN_FOUND)

if (DOXYGEN_FOUND AND SPHINX_EXECUTABLE)
add_custom_target(docs  
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    COMMAND
              ${SPHINX_EXECUTABLE} -M html
              ${SPHINX_SOURCE} ${SPHINX_BUILD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Doxygen and Sphinx documentation"
    VERBATIM )
endif()

endif (BUILD_DOC)

