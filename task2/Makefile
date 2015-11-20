CXX:=g++
CXXFLAGS:=-std=c++11 -g -O3 -Wall
LIBS := -ltbb -lstdc++
BUILD:=.
FILES:=.

all: parser tpcc

Schema.o: $(FILES)/Schema.cpp $(FILES)/Schema.hpp Makefile
	$(CXX) $(CXXFLAGS) -c $(FILES)/Schema.cpp -o $(BUILD)/Schema.o

Parser.o: $(FILES)/Parser.cpp $(FILES)/Parser.hpp Makefile
	$(CXX) $(CXXFLAGS) -c $(FILES)/Parser.cpp -o $(BUILD)/Parser.o
	
Types.o: $(FILES)/Types.cpp $(FILES)/Types.hpp Makefile
	$(CXX) $(CXXFLAGS) -c -fPIC $(FILES)/Types.cpp -o $(BUILD)/Types.o
		
Database.o: $(FILES)/schema_schema_generated.hpp $(FILES)/schema_schema_generated.cpp $(FILES)/Types.hpp Makefile
	$(CXX) $(CXXFLAGS) -c -fPIC $(FILES)/schema_schema_generated.cpp -o $(BUILD)/Database.o	
	
parser: $(FILES)/Schema.o $(FILES)/Parser.o $(FILES)/Schema.hpp $(FILES)/SchemaTypes.hpp $(FILES)/main.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Parser.o $(BUILD)/Schema.o $(FILES)/main.cpp -o $(BUILD)/parser
	
tpcc:   Database.o $(FILES)/Types.o $(FILES)/Types.hpp $(FILES)/schema_schema_generated.hpp $(FILES)/query5_query_generated.hpp $(FILES)/transactions.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Types.o $(BUILD)/Database.o $(FILES)/transactions.cpp -o $(BUILD)/tpcc $(LIBS) -Wno-unused-function -Wno-unused-variable

operator: $(FILES)/Schema.o $(FILES)/Parser.o $(FILES)/Schema.hpp $(FILES)/SchemaTypes.hpp $(FILES)/querygenerator.cpp $(FILES)/operator.hpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Parser.o $(BUILD)/Schema.o $(FILES)/querygenerator.cpp -o $(BUILD)/operator
	
uni:  $(BUILD)/Types.o $(FILES)/Types.hpp $(FILES)/uni_schema_generated.hpp $(FILES)/uni.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Types.o $(FILES)/uni.cpp -o $(BUILD)/uni $(LIBS) -Wno-unused-function -Wno-unused-variable

interpreter:  $(BUILD)/Parser.o $(BUILD)/Schema.o $(BUILD)/Database.o $(BUILD)/Types.o $(FILES)/operator.hpp  $(FILES)/Parser.hpp  $(FILES)/Parser.cpp $(FILES)/Types.hpp $(FILES)/interpreter.cpp Makefile
	$(CXX) $(CXXFLAGS) $(BUILD)/Database.o $(BUILD)/Parser.o $(BUILD)/Schema.o $(BUILD)/Types.o $(FILES)/interpreter.cpp -o $(BUILD)/interpreter $(LIBS) -lrt -ldl -Wno-unused-function -Wno-unused-variable

#dont use O3 on this one
sharedquery:  Types.o
	$(CXX) -std=c++11 -g -O0 -Wall $(BUILD)/Types.o $(BUILD)/Database.o $(FILES)/schema_query_generated.cpp -shared -fPIC  -o $(BUILD)/query.so

clean:
	rm -rf *.o *.so $(BUILD)/parser $(BUILD)/tpcc $(BUILD)/operator $(BUILD)/uni $(BUILD)/interpreter
