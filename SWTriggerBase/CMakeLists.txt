project (SWTriggerBase)

# Now set CMAKE variables for my use
set(SHLIB "libSWTriggerBase.so")
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
  set(DICT_NAME "SWTriggerBaseDict")
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
		  -rmf ${DAQ_TRIGGER_ALGO_LIBDIR}/libSWTriggerBase.rootmap
		  -c ${CINTFLAGS} -I${ROOT_INCDIR} -I.
		  ${HEADERS} ${LINKDEF})
  ELSE()
  execute_process(COMMAND rootcint
  		  -f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx 
		  -c -I${ROOT_INCDIR} -I. 
		  -p  ${HEADERS} ${LINKDEF})

  ENDIF()
  include_directories(${ROOT_INCDIR})
  add_library(SWTriggerBase SHARED ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  target_link_libraries(SWTriggerBase ${ROOT_LIB} )
  set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DAQ_TRIGGER_ALGO_LIBDIR}/libSWTriggerBase.rootmap" )
ELSE()

  add_library(SWTriggerBase SHARED ${SOURCES} )
  target_link_libraries(SWTriggerBase)
ENDIF()

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-g -gdwarf-2 -O0")
set(CMAKE_CXX_FLAGS_PROF "-g -gdwarf-2 -O3")
set(CMAKE_CXX_FLAGS_OPT "-O3")
set_target_properties( SWTriggerBase
		       PROPERTIES
	               LIBRARY_OUTPUT_DIRECTORY ${DAQ_TRIGGER_ALGO_LIBDIR}
)

if (NOT ${ROOT6})

  add_custom_command( TARGET SWTriggerBase
		      POST_BUILD
		      COMMAND rlibmap ARGS -f
		      -o ${DAQ_TRIGGER_ALGO_LIBDIR}/libSWTriggerBase.rootmap
		      -l ${DAQ_TRIGGER_ALGO_LIBDIR}/libSWTriggerBase.so
		      -c ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
endif()

if (APPLE)
   set_property(TARGET SWTriggerBase PROPERTY PREFIX "lib")
   set_property(TARGET SWTriggerBase PROPERTY OUTPUT_NAME "SWTriggerBase.so")
   set_property(TARGET SWTriggerBase PROPERTY SUFFIX "")
endif()


