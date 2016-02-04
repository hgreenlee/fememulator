string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Yellow      "${Esc}[33m")

cmake_minimum_required (VERSION 2.8)
project (DAQTriggerAlgo)

set(USE_ROOT 0)
IF(DEFINED ENV{SWTRIGGER_ROOT6} OR DEFINED ENV{SWTRIGGER_ROOT5})
  set(USE_ROOT 1)
  execute_process(COMMAND root-config --incdir 
		  OUTPUT_VARIABLE ROOT_INCDIR 
		  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND root-config --libs 
		  OUTPUT_VARIABLE ROOT_LIB 
		  OUTPUT_STRIP_TRAILING_WHITESPACE)
  IF(DEFINED ENV{SWTRIGGER_ROOT6})
    set(ROOT6 1)
    message("--${Yellow} Using ROOT6${ColorReset}")
  ELSE()
    set(ROOT6 0)
    message("--${Yellow} Using ROOT5${ColorReset}")
  ENDIF()
ELSE()
  message("--${Yellow} no ROOT${ColorReset}")
ENDIF()

set(DAQ_TRIGGER_ALGO_BUILDDIR ${CMAKE_SOURCE_DIR}/build)
set(DAQ_TRIGGER_ALGO_INCDIR ${CMAKE_SOURCE_DIR})

IF(DEFINED ENV{LARLITE_LIBDIR})

  message("-- ${Yellow}Building with LArLite${ColorReset}")
  execute_process(COMMAND larlite-config --incdir 
		  OUTPUT_VARIABLE LARLITE_INC 
		  OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND larlite-config --libs 
		  OUTPUT_VARIABLE LARLITE_LIB 
		  OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(DAQ_TRIGGER_ALGO_LIBDIR $ENV{LARLITE_LIBDIR})

  add_subdirectory(LArLiteApp)
ELSE()
  set(DAQ_TRIGGER_ALGO_LIBDIR ${CMAKE_SOURCE_DIR}/build/lib)
ENDIF()

execute_process(COMMAND mkdir -p ${DAQ_TRIGGER_ALGO_LIBDIR})

add_subdirectory(SWTriggerBase)
add_subdirectory(FEMBeamTrigger)

IF(DEFINED ENV{UBOONEDAQ_DATATYPES_INC})

  message("-- ${Yellow}Building daq file interface${ColorReset}")
  set(BOOST_INC $ENV{BOOST_INC})
  set(UBDAQ_DATATYPES_INC $ENV{UBOONEDAQ_DATATYPES_INC})
  set(BOOST_LIB "-L$ENV{BOOST_LIB} -lboost_serialization")
  set(UBDAQ_DATATYPES_LIB "-L$ENV{UBOONEDAQ_DATATYPES_LIB} -lubdata_types")

  add_subdirectory(TriggerDAQApp)
ENDIF()
