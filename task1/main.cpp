#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <type_traits>
#include <typeinfo>
#include "schema.hpp"
#include "oltp.hpp"
using namespace std;



uint64_t warehouse::count = 0;
warehouse* warehouse::relation = NULL;

uint64_t item::count = 0;
item* item::relation = NULL;

// no partial function template specialisation in c++11 -> workaround
template <typename S, unsigned N>
struct Loader
{
    static void crazyTemplateLoader (istringstream & ss, string & line, S & t)
    {
        
        string token;
        getline ( ss, token, '|' );
        using base = decltype(S::TUPLE);
        using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
        std::get<std::tuple_size<base>::value - N>(t.TUPLE) = type::castString(token.c_str(), token.size() );
        Loader<S, N - 1>::crazyTemplateLoader (ss, line, t);
    }

};
template<typename S>
struct Loader<S, 0>
{
    static void crazyTemplateLoader (istringstream & ss,  string line, S & Tuple)
    {
        // do nothing -> recursion end
    }
};


template<typename r>
void load_relation()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string(r::name) + ".tbl" );
    r::count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    r::relation = new r[r::count];

    if ( myfile.is_open() )
    {
        uint64_t l_number = 0;
        while ( getline ( myfile,line ) )
        {
            istringstream ss ( line );
            Loader<r, std::tuple_size<decltype(r::TUPLE)>::value>::crazyTemplateLoader(ss, line, r::relation[l_number]);
            l_number++;
        }
        myfile.close();
    }
}





int main ( int argc, char **argv )
{
    
    
    load_relation<warehouse>();
    warehouse::print_relation();
    
    //load_relation<item>();
    //item::print_relation();

    cout <<  sizeof(warehouse) <<  endl;
    //wa_load();

    //it_load();
    //wa_print();
    //it_print();
    return 0;
}


