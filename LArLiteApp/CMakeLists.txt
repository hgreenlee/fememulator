project (FEMBeamTrigger_LArLiteInterface)

# Now set CMAKE variables for my use
set(SHLIB "libFEMBeamTrigger_LArLiteInterface.so")
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
  set(DICT_NAME "FEMBeamTrigger_LArLiteInterfaceDict")
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
		  -rmf ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger_LArLiteInterface.rootmap
		  -c ${CINTFLAGS} -I${ROOT_INCDIR} -I${LARLITE_INC} -I${DAQ_TRIGGER_ALGO_INCDIR} -I.
		  ${HEADERS} ${LINKDEF})
  ELSE()
  execute_process(COMMAND rootcint
  		  -f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx 
		  -c -I${ROOT_INCDIR} -I${LARLITE_INC} -I${DAQ_TRIGGER_ALGO_INCDIR} -I. 
		  -p  ${HEADERS} ${LINKDEF})

  ENDIF()
  include_directories(${ROOT_INCDIR})
  add_library(FEMBeamTrigger_LArLiteInterface SHARED ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  target_link_libraries(FEMBeamTrigger_LArLiteInterface SWTriggerBase FEMBeamTrigger ${ROOT_LIB} ${LARLITE_LIB} )
  set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger_LArLiteInterface.rootmap" )
ELSE()

  add_library(FEMBeamTrigger_LArLiteInterface SHARED ${SOURCES} )
  target_link_libraries(FEMBeamTrigger_LArLiteInterface SWTriggerBase FEMBeamTrigger)
ENDIF()

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${DAQ_TRIGGER_ALGO_INCDIR})
include_directories(${LARLITE_INC})

set(CMAKE_CXX_FLAGS "-std=c++11")
set_target_properties( FEMBeamTrigger_LArLiteInterface
		       PROPERTIES
                       LIBRARY_OUTPUT_DIRECTORY ${DAQ_TRIGGER_ALGO_LIBDIR}
)

if (NOT ${ROOT6})

  add_custom_command( TARGET FEMBeamTrigger_LArLiteInterface
		      POST_BUILD
		      COMMAND rlibmap ARGS -f
		      -o ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger_LArLiteInterface.rootmap
		      -l ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger_LArLiteInterface.so
		      -c ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
endif()

if (APPLE)
   set_property(TARGET FEMBeamTrigger_LArLiteInterface PROPERTY PREFIX "lib")
   set_property(TARGET FEMBeamTrigger_LArLiteInterface PROPERTY OUTPUT_NAME "FEMBeamTrigger_LArLiteInterface.so")
   set_property(TARGET FEMBeamTrigger_LArLiteInterface PROPERTY SUFFIX "")
endif()


