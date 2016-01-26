
#FEMEMU_HOME = /Users/twongjirad/working/uboone/fememulator
FEMEMU_HOME = /home/tmw/fememulator

#CXXFLAGS = -g -Wc++11-extensions
CXXFLAGS = -g -std=c++1y `root-config --cflags`
INCFLAGS = -I${BOOST_INC} -I${UBOONEDAQ_DATATYPES_INC}
LDLIBS = -L${FEMEMU_HOME}/build/lib -lFEMBeamTrigger -lSWTriggerBase
DAQ_LDLIBS += -lTriggerDAQApp
DAQ_LDLIBS += -L${UBOONEDAQ_DATATYPES_LIB} -lubdata_types
DAQ_LDLIBS += -L${BOOST_LIB} -lboost_wserialization
ROOTLIBS = `root-config --libs`

all: test_app validate_swtrigs

test_app.o: test_app.cc
	g++ $(CXXFLAGS) -I./../ -c test_app.cc -o test_app.o

test_app: test_app.o
	g++ $(CXXFLAGS) -o test_app test_app.o $(LDLIBS)
	@rm test_app.o

validate_swtrigs.o: validate_swtrigs.cc
	g++ $(CXXFLAGS) -I./../ $(INCFLAGS) -c validate_swtrigs.cc -o validate_swtrigs.o

validate_swtrigs: validate_swtrigs.o
	g++ $(CXXFLAGS) -o validate_swtrigs validate_swtrigs.o $(LDLIBS) $(DAQ_LDLIBS) $(ROOTLIBS)
	@rm validate_swtrigs.o

clean:
	rm test_app validate_swtrigs