

CXXFLAGS = -g -Wc++11-extensions
LDLIBS = -L/Users/twongjirad/working/uboone/fememulator/build/lib -lFEMBeamTrigger -lSWTriggerBase



all: test_app
	

test_app.o: test_app.cc
	g++ $(CXXFLAGS) -I./../ -c test_app.cc -o test_app.o

test_app: test_app.o
	g++ $(CXXFLAGS) -o test_app test_app.o $(LDLIBS)
	@rm test_app.o

clean:
	rm test_app