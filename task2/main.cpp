#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include "Parser.hpp"
#include "Schema.hpp"

int main ( int argc, char* argv[] )
{
    if (argc != 2) {
       std::cerr << "usage: " << argv[0] << " <schema name>" << std::endl;
       return -1;
    }

    std::string tables("../tpc-c/schema.sql");
    std::string c("../tpc-c/"+std::string(argv[1])+".sql");

    Parser p ( tables.c_str() );
    try
    {
        std::unique_ptr<Schema> schema;
        schema = p.parse();
		std::ifstream in;
		in.open(c);
		if (!in.is_open())
			throw ParserError(1, "cannot open file '"+c+"'");
		p.parse(*schema,in);
		auto& head = p.tree.operators.front();
		p.tree.print(head);
        
        //std::cout << schema->relations.at(0).name;
		//std::cout << schema->toCpp() << std::endl;
        /*std::ofstream hpp_gen;
        hpp_gen.open ( std::string(argv[1])+"_schema_generated.hpp" );
        hpp_gen << schema->toHpp();
        hpp_gen.close();
		
		std::ofstream cpp_gen;
        cpp_gen.open ( std::string(argv[1])+"_schema_generated.cpp" );
        cpp_gen << "#include \"" << std::string(argv[1]) << "_schema_generated.hpp\"\n";
		cpp_gen << schema->toCpp();
        cpp_gen.close();*/
    }
    catch ( ParserError& e )
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; 
