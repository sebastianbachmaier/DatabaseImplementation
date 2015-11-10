CXX:=g++
CXXFLAGS:=-std=c++11 -g -O0 -Wall
LIBS := -ltbb
BUILD:=.
FILES:=.

all: parser tpcc

Schema.o: $(FILES)/Schema.cpp $(FILES)/Schema.hpp Makefile
	$(CXX) $(CXXFLAGS) -c $(FILES)/Schema.cpp -o $(BUILD)/Schema.o

Parser.o: $(FILES)/Parser.cpp $(FILES)/Parser.hpp Makefile
	$(CXX) $(CXXFLAGS) -c $(FILES)/Parser.cpp -o $(BUILD)/Parser.o
	
Types.o: $(FILES)/Types.cpp $(FILES)/Types.hpp Makefile
	$(CXX) $(CXXFLAGS) -c $(FILES)/Types.cpp -o $(BUILD)/Types.o

parser: $(FILES)/Schema.o $(FILES)/Parser.o $(FILES)/Schema.hpp $(FILES)/SchemaTypes.hpp $(FILES)/main.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Parser.o $(BUILD)/Schema.o $(FILES)/main.cpp -o $(BUILD)/parser
	
tpcc:  $(FILES)/Types.o $(FILES)/Types.hpp $(FILES)/schema_generated.hpp $(FILES)/transactions.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Types.o $(FILES)/transactions.cpp -o $(BUILD)/tpcc $(LIBS) -Wno-unused-function -Wno-unused-variable

operator: $(FILES)/Schema.o $(FILES)/Parser.o $(FILES)/Schema.hpp $(FILES)/SchemaTypes.hpp $(FILES)/querygenerator.cpp $(FILES)/operator.hpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Parser.o $(BUILD)/Schema.o $(FILES)/querygenerator.cpp -o $(BUILD)/operator
	
clean:
	rm -rf *.o $(BUILD)/parser $(BUILD)/tpcc $(BUILD)/operator
