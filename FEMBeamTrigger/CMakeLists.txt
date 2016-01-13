string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Yellow      "${Esc}[33m")

cmake_minimum_required (VERSION 2.8)
project (FEMBeamTrigger)

execute_process(COMMAND root-config --incdir OUTPUT_VARIABLE ROOT_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python -c "import ROOT;print int(ROOT.gROOT.GetVersion()[0] == '6')" OUTPUT_VARIABLE USE_ROOT6 OUTPUT_STRIP_TRAILING_WHITESPACE)

# Now set CMAKE variables for my use
set(SHLIB "libFEMBeamTrigger.so")
set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "./lib")
set(DICT_NAME "FEMBeamTriggerDict")
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
  MESSAGE("-- Removing a dictionary file: ${Yellow}${DICT_NAME}.cxx${ColorReset}")
  file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
endif()
if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
  MESSAGE("-- Removing a dictionary file: ${Yellow}${DICT_NAME}.h${ColorReset}")
  file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.h)
endif()

# Get list of course
file( GLOB HEADERS *.h  )
file( GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cxx  )

file( GLOB LINKDEF LinkDef.h )
list( GET LINKDEF 0 LINKDEF )
list( REMOVE_ITEM HEADERS ${LINKDEF} )

# Pre process to generate dictionary source code + getting compiler/linker flags
IF(${USE_ROOT6})
execute_process(COMMAND rootcling
		-f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx
		-s ${DAQ_TRIGGER_ALGO_LIBDIR}/${SHLIB}
		-rml ${SHLIB}
		-rmf ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger.rootmap
		-c ${CINTFLAGS} -I${ROOT_INCDIR} -I${DAQ_TRIGGER_ALGO_INCDIR} -I.
		${HEADERS} ${LINKDEF})
ELSE()
execute_process(COMMAND rootcint
		-f ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx 
		-c -I${ROOT_INCDIR} -I${DAQ_TRIGGER_ALGO_INCDIR} -I. 
		-p  ${HEADERS} ${LINKDEF}
		)
ENDIF()

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${ROOT_INCDIR})
include_directories(${DAQ_TRIGGER_ALGO_INCDIR})
set(CMAKE_CXX_FLAGS "-std=c++11")

# Generate a package library ... make sure dictionary comes the end
add_library(FEMBeamTrigger SHARED ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/${DICT_NAME}.cxx)
target_link_libraries(FEMBeamTrigger ${ROOT_LIB} -L${DAQ_TRIGGER_ALGO_LIBDIR} -lSWTriggerBase )
add_dependencies(FEMBeamTrigger SWTriggerBase)

set_target_properties( FEMBeamTrigger
		       PROPERTIES
	               LIBRARY_OUTPUT_DIRECTORY ${DAQ_TRIGGER_ALGO_LIBDIR}
)
if (APPLE)
   set_property(TARGET FEMBeamTrigger PROPERTY PREFIX "lib")
   set_property(TARGET FEMBeamTrigger PROPERTY OUTPUT_NAME "FEMBeamTrigger.so")
   set_property(TARGET FEMBeamTrigger PROPERTY SUFFIX "")
endif()


# Generate a rootmap file
IF(NOT ${USE_ROOT6}) 
add_custom_command( TARGET FEMBeamTrigger
		    POST_BUILD
		    COMMAND rlibmap ARGS -f
		    -o ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger.rootmap
		    -l ${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger.so
		    -c ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
)
ENDIF()
set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DAQ_TRIGGER_ALGO_LIBDIR}/libFEMBeamTrigger.rootmap" )

