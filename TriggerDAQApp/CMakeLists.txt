project (TriggerDAQApp)

# Now set CMAKE variables for my use
set(SHLIB "libTriggerDAQApp.so")
if(APPLE)
  set(CMAKE_MACOSX_RPATH TRUE)
endif()

# Get list of sourse
file( GLOB HEADERS *.h  )
file( GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cxx  )

file( GLOB LINKDEF LinkDef.h )
list( GET LINKDEF 0 LINKDEF )
list( REMOVE_ITEM HEADERS ${LINKDEF} )

IF(USE_ROOT)
  set(DICT_NAME "TriggerDAQAppDict")
  IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
    MESSAGE("-- Removing a dictionary file: ${Yellow}${DICT_NAME}.cxx${ColorReset}")
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  ENDIF()
  IF(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
    MESSAGE("-- Removing a dictionary file: ${Yellow}${DICT_NAME}.h${ColorReset}")
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
  ENDIF()

  # Pre process to generate dictionary source code + getting compiler/linker flags
  IF(${ROOT6})
  execute_process(COMMAND rootcling
  		  -f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx
		  -s ${DAQ_TRIGGER_ALGO_LIBDIR}/${SHLIB}
		  -rml ${SHLIB}
		  -rmf ${DAQ_TRIGGER_ALGO_LIBDIR}/libTriggerDAQApp.rootmap
		  -c ${CINTFLAGS} -I${ROOT_INCDIR} -I${DAQ_TRIGGER_ALGO_INCDIR} -I${UBDAQ_DATATYPES_INC} -I. 
		  ${HEADERS} ${LINKDEF})
  ELSE()
  execute_process(COMMAND rootcint
  		  -f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx 
		  -c -I${ROOT_INCDIR} -I${DAQ_TRIGGER_ALGO_INCDIR} -I${UNDAQ_DATATYPES_INC} -I. 
		  -p  ${HEADERS} ${LINKDEF})

  ENDIF()
  include_directories(${ROOT_INCDIR})
  add_library(TriggerDAQApp SHARED ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  target_link_libraries(TriggerDAQApp SWTriggerBase FEMBeamTrigger ${ROOT_LIB} ${BOOST_LIB} ${UBDAQ_DATATYPES_LIB})
  set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DAQ_TRIGGER_ALGO_LIBDIR}/libTriggerDAQApp.rootmap" )
ELSE()
  add_library(TriggerDAQApp SHARED ${SOURCES} )
  target_link_libraries(TriggerDAQApp SWTriggerBase ${BOOST_LIB} ${UBDAQ_DATATYPES_LIB})
ENDIF()

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${DAQ_TRIGGER_ALGO_INCDIR})
include_directories(${BOOST_INC})
include_directories(${UBDAQ_DATATYPES_INC})

set(CMAKE_CXX_FLAGS "-std=c++1y")
set_target_properties( TriggerDAQApp
		       PROPERTIES
	               LIBRARY_OUTPUT_DIRECTORY ${DAQ_TRIGGER_ALGO_LIBDIR}
)

if (NOT ${ROOT6})

  add_custom_command( TARGET TriggerDAQApp
		      POST_BUILD
		      COMMAND rlibmap ARGS -f
		      -o ${DAQ_TRIGGER_ALGO_LIBDIR}/libTriggerDAQApp.rootmap
		      -l ${DAQ_TRIGGER_ALGO_LIBDIR}/libTriggerDAQApp.so
		      -c ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
endif()

if (APPLE)
   set_property(TARGET TriggerDAQApp PROPERTY PREFIX "lib")
   set_property(TARGET TriggerDAQApp PROPERTY OUTPUT_NAME "TriggerDAQApp.so")
   set_property(TARGET TriggerDAQApp PROPERTY SUFFIX "")
endif()


