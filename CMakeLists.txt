string(ASCII 27 Esc)
set(ColorReset "${Esc}[m")
set(Yellow      "${Esc}[33m")

cmake_minimum_required (VERSION 2.8)
project (DAQTriggerAlgo)

execute_process(COMMAND root-config --incdir OUTPUT_VARIABLE ROOT_INCDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND python -c "import ROOT;print int(ROOT.gROOT.GetVersion()[0] == '6')" OUTPUT_VARIABLE USE_ROOT6 OUTPUT_STRIP_TRAILING_WHITESPACE)

IF(${USE_ROOT6})
message("-- ${Yellow}Using ROOT6${ColorReset}")
ELSE()
message("-- ${Yellow}Using ROOT5${ColorReset}")
ENDIF()

set(DAQ_TRIGGER_ALGO_BUILDDIR ${CMAKE_SOURCE_DIR}/build)
set(DAQ_TRIGGER_ALGO_INCDIR ${CMAKE_SOURCE_DIR})
#set(LARLITE_LIBDIR $ENV{LARLITE_LIBDIR})

IF(DEFINED ENV{LARLITE_LIBDIR})

message("-- ${Yellow}Building with LArLite${ColorReset}")
execute_process(COMMAND larlite-config --incdir OUTPUT_VARIABLE LARLITE_INC OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND larlite-config --libs OUTPUT_VARIABLE LARLITE_LIB OUTPUT_STRIP_TRAILING_WHITESPACE)
set(DAQ_TRIGGER_ALGO_LIBDIR $ENV{LARLITE_LIBDIR})
execute_process(COMMAND mkdir -p ${DAQ_TRIGGER_ALGO_LIBDIR})
add_subdirectory(FEMBeamTrigger)
add_subdirectory(LArLiteApp)

ELSE()

message("-- ${Yellow}Building stand-alone${ColorReset}")
set(DAQ_TRIGGER_ALGO_LIBDIR ${CMAKE_SOURCE_DIR}/build/lib)
execute_process(COMMAND mkdir -p ${DAQ_TRIGGER_ALGO_LIBDIR})
add_subdirectory(FEMBeamTrigger)

ENDIF()




