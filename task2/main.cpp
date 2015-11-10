#include <iostream>
#include <memory>
#include <fstream>
#include "Parser.hpp"
#include "Schema.hpp"

int main ( int argc, char* argv[] )
{
    /*if (argc != 2) {
       std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
       return -1;
    }*/

    const char* c = "../tpc-c/schema.sql";

    Parser p ( c );
    try
    {
        std::unique_ptr<Schema> schema;
        schema = p.parse();
        
        std::cout << schema->relations.at(0).name;
		// std::cout << schema->toCpp() << std::endl;
        //std::ofstream schema_generated;
        //schema_generated.open ( "schema_generated.hpp" );
        //schema_generated << schema->toCpp();
        //schema_generated.close();
    }
    catch ( ParserError& e )
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; 
