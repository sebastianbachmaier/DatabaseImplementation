#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include "schema_schema_generated.hpp"
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>
#include <sys/mman.h>
#include <math.h>
#include <mutex>
#include <stdint.h>
#include <stdint-gcc.h>
#include <signal.h>
#include "Parser.hpp"
#include "Schema.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <dlfcn.h>


using namespace std;
using namespace tbb;
using namespace std::chrono;

int main(void)
{
    DATABASE db;
    
    srand ( time ( NULL ) );
    
    cout <<  "loading tbl..." << endl;
    auto start=high_resolution_clock::now();
    db.init_tbl();
    cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
    
    std::string tables("../tpc-c/schema.sql");
    std::string c("../tpc-c/query.sql");
    

 // How to get a string/sentence with spaces
    
    //getline(cin, sql);
    
    std::string queryString;
    Parser pSchema ( tables.c_str() );
    std::unique_ptr<Schema> schema;
    schema = pSchema.parse();
    
    while (1)
    {
        try{

                /*std::ifstream in;
                in.open(c);
                if (!in.is_open())
                    throw ParserError(1, "cannot open file '"+c+"'");*/
                Parser p ( "" );
                std::string input;                          // = "select o_id, ol_dist_info from order, orderline where o_id = ol_o_id and ol_d_id = o_d_id and o_w_id = ol_w_id and ol_number = 1 and ol_o_id = 100;";
                //sql << in.rdbuf();
                cout << "SQL (enter 'exit;' to terminate, querys end with ';'):\n>";
                std::cin.clear();
                input.clear();
                
                getline(std::cin, input, ';');
                    
                std::cout << input << std::endl;
                std::size_t found = input.find("exit");
                if (found!=std::string::npos)
                    return 0;
                std::stringstream sql(input+";");
                //in << sql;
                p.parse(*schema,sql);
                auto& head = p.tree.operators.front();
                p.tree.optimizeQuery(head, *schema);
                queryString = p.tree.produce(head);
            
            
            start=high_resolution_clock::now();
            cout << "generate query...\n";

            std::ofstream query_generated;
            query_generated.open ( "schema_query_generated.cpp" );
            std::string cppFile = "\
            #include \"schema_schema_generated.hpp\"\n\
            ";
            query_generated << cppFile;
            query_generated << queryString;
            query_generated.close();
            
            cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
        
            start=high_resolution_clock::now();
            cout << "compile query...\n";
            if(!vfork()){
                //execlp("g++","g++","-O3", "-std=c++11", "-g", (input + "_query_generated.cpp").c_str(),"-lm", "-shared","-fPIC", "-o", "query.so", nullptr);
                execlp("make", "make","--silent", "sharedquery", "-j4", nullptr);
            }   
            wait(NULL);
            cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
        
            start=high_resolution_clock::now();
            cout << "run query...\n\n";
            
            void* handle=dlopen("./query.so",RTLD_NOW);
            if (!handle) {
                cerr << "error: " << dlerror() << endl;
                exit(1);
            }

            auto query=reinterpret_cast<std::string (*)(DATABASE *)>(dlsym(handle, "query"));
            if (!query) {
                cerr << "error: " << dlerror() << endl;
                exit(1);
            }

            cout << query(&db) << endl;

            if (dlclose(handle)) {
                cerr << "error: " << dlerror() << endl;
                exit(1);
            }
            cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
        }catch ( ParserError& e ) {
            std::cerr << e.what() << std::endl;
        }

    }
    return 0;
}
                