set(SCCL systemc-clang)

set(sccl_SOURCES
  driver-tooling.cpp
  )

add_library(model
 ./model/EventDecl.cpp
./model/PortDecl.cpp
./model/Model.cpp
./model/NotifyContainer.cpp
./model/ProcessDecl.cpp
./model/EventContainer.cpp
./model/InterfaceDecl.cpp
./model/ModuleInstance.cpp
./model/SignalDecl.cpp
./model/WaitCalls.cpp
./model/WaitContainer.cpp
./model/EntryFunctionContainer.cpp
./model/NotifyCalls.cpp
)


add_library(cfg
  ./cfg/SplitCFGBlock.cpp
./cfg/SplitCFG.cpp
)

add_library( matchers 
./matchers/FindEvents.cpp
./matchers/FindEntryFunctions.cpp
./matchers/FindGlobalEvents.cpp
./matchers/FindNotify.cpp
./matchers/FindConstructor.cpp
./matchers/FindSCMain.cpp
./matchers/FindNetlist.cpp
./matchers/FindTemplateParameters.cpp
./matchers/FindArgument.cpp
./matchers/FindWait.cpp
./matchers/FindSimTime.cpp
./matchers/FindTemplateTypes.cpp
)

add_library( utils
./utils/CXXRecordDeclUtils.cpp
./utils/CallExprUtils.cpp
./utils/ArrayTypeUtils.cpp
)


add_library(hdl
  ${CMAKE_SOURCE_DIR}/plugins/hdl/HDLMain.cpp
  ${CMAKE_SOURCE_DIR}/plugins/hdl/HDLBody.cpp
  ${CMAKE_SOURCE_DIR}/plugins/hdl/HDLType.cpp
  ${CMAKE_SOURCE_DIR}/plugins/hdl/HDLHnode.cpp
  ${CMAKE_SOURCE_DIR}/plugins/hdl/HDLThread.cpp
  )

add_library(sccl SystemCClang.cpp)

 target_link_libraries(sccl
   model
   matchers
   cfg 
   utils
   )

include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}/
      ${CMAKE_CURRENT_SOURCE_DIR}/model/
      ${CMAKE_CURRENT_SOURCE_DIR}/cfg/
      ${CMAKE_CURRENT_SOURCE_DIR}/SCuitable/
      ${CMAKE_CURRENT_SOURCE_DIR}/utils
      ${CMAKE_CURRENT_SOURCE_DIR}/matchers/
      ${CMAKE_CURRENT_SOURCE_DIR}/plugins/hdl/
      )

# CONFIGURE THE TOOLS
# ===================
foreach( tool ${SCCL} )
    # Create a library corresponding to 'plugin'
    add_executable(
      ${tool}
      ${CMAKE_SOURCE_DIR}/driver-hdl.cpp # ${${tool}_SOURCES}
      )

    # Configure include directories for 'tool'
    target_include_directories(
      ${tool}
      PRIVATE
      ${CMAKE_SOURCE_DIR}/plugins/hdl/
      ${CMAKE_CURRENT_SOURCE_DIR}/
      ${CMAKE_CURRENT_SOURCE_DIR}/model/
      ${CMAKE_CURRENT_SOURCE_DIR}/cfg/
      ${CMAKE_CURRENT_SOURCE_DIR}/SCuitable/
      ${CMAKE_CURRENT_SOURCE_DIR}/utils
      ${CMAKE_CURRENT_SOURCE_DIR}/matchers/
    )

    # Link in the required libraries 
    target_link_libraries(
     ${tool}
     sccl
     hdl
     "clangTooling"
    )
endforeach()
