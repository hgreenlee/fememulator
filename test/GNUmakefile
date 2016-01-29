
#FEMEMU_HOME = /Users/twongjirad/working/uboone/fememulator
#FEMEMU_HOME = /uboone/app/users/kterao/TMWEmulator
FEMEMU_HOME=$(SWTRIGGER_INCDIR)
CXXFLAGS = -g -std=c++1y `root-config --cflags`
INCFLAGS = -I${BOOST_INC} -I${UBOONEDAQ_DATATYPES_INC}
LDLIBS = -L${FEMEMU_HOME}/build/lib -lFEMBeamTrigger -lTriggerDAQApp -lSWTriggerBase 
DAQ_LDLIBS += -L${UBOONEDAQ_DATATYPES_LIB} -lubdata_types
DAQ_LDLIBS += -L${BOOST_LIB} -lboost_wserialization
ROOTLIBS = `root-config --libs`

all: test_app validate_swtrigs

test_app.o: test_app.cc
	@echo "<<compiling test_app>>"
	@g++ $(CXXFLAGS) -I./../ -c test_app.cc -o test_app.o

test_app: test_app.o
	@g++ $(CXXFLAGS) -o test_app.exe test_app.o $(LDLIBS)

validate_swtrigs.o: validate_swtrigs.cc
	@echo "<<compiling validate_swtrigs>>"
	@g++ $(CXXFLAGS) -I./../ $(INCFLAGS) -c validate_swtrigs.cc -o validate_swtrigs.o

validate_swtrigs: validate_swtrigs.o
	@g++ $(CXXFLAGS) -o validate_swtrigs.exe validate_swtrigs.o $(LDLIBS) $(DAQ_LDLIBS) $(ROOTLIBS)

clean:
	@rm -f *.o
	@rm -f test_app.exe validate_swtrigs.exe

