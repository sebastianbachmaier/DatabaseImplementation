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


#if 0

// this could have been generic
/*
item
warehouse
district
customer
history
neworder
order
orderline
stock
*/

uint64_t item::count = 0;
uint64_t warehouse::count = 0;
uint64_t district::count = 0;
uint64_t customer::count = 0;
uint64_t history::count = 0;
uint64_t neworder::count = 0;
uint64_t order::count = 0;
uint64_t orderline::count = 0;
uint64_t stock::count = 0;

item* item::relation = NULL;
warehouse* warehouse::relation = NULL;
district* district::relation = NULL;
customer* customer::relation = NULL;
history* history::relation = NULL;
neworder* neworder::relation = NULL;
order* order::relation = NULL;
orderline* orderline::relation = NULL;
stock* stock::relation = NULL;

item::p_type item::COLUMNS {NULL, NULL, NULL, NULL, NULL};
std::tuple<Integer*, Varchar<10>*, Varchar<20>*, Varchar<20>*, Varchar<20>*,
    Char<2>*, Char<9>*, Numeric<4, 4>*, Numeric<4, 4>*> warehouse::COLUMNS;
std::tuple<Integer*, Integer*, Varchar<10>*, Varchar<20>*, Varchar<20>*, Varchar<20>*,
    Char<2>*, Char<9>*, Numeric<4, 4>*, Numeric<12, 2>*, Integer*> district::COLUMNS;
std::tuple<Integer*, Integer*, Integer*, Varchar<16>*, Char<2>*, Varchar<16>*, Varchar<20>*, Varchar<20>*, Varchar<20>*,
    Char<2>*, Char<9>*, Char<16>*, Timestamp*, Char<2>*, Numeric<12, 2>*, Numeric<4, 4>*, Numeric<12, 2>*, Numeric<12, 2>*,
    Numeric<4, 0>*, Numeric<4, 0>*, Varchar<500>*> customer::COLUMNS;
std::tuple<Integer*, Integer*, Integer*, Integer*, Integer*, Timestamp*, Numeric<6, 2>*, Varchar<24>*> history::COLUMNS {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
std::tuple<Integer*, Integer*, Integer*> neworder::COLUMNS {NULL, NULL, NULL};
std::tuple<Integer*, Integer*, Integer*, Integer*, Timestamp*, Integer*, Numeric<2, 0>*, Numeric<1, 0>*> order::COLUMNS;
std::tuple<Integer*, Integer*, Integer*, Integer*, Integer*, Integer*,Timestamp*, Numeric<2, 0>*, Numeric<6, 2>*, Char<24>*> orderline::COLUMNS;
std::tuple<Integer*, Integer*, Numeric<4, 0>*, Char<24>*, Char<24>*, Char<24>*,
    Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*,
    Numeric<8, 0>*, Numeric<4, 0>*,Numeric<4, 0>*,Varchar<50>*> stock::COLUMNS;


std::map<index::primary_key_type,uint64_t> index::primary_index;


// no partial function template specialisation in c++11 -> workaround
template <typename S, unsigned N>
struct Loader
{
    static void crazyTemplateLoader ( istringstream & ss, string & line, uint64_t l_number )
    {
        string token;
        getline ( ss, token, '|' );
        if ( S::columnstore )
        {
            using base = decltype ( S::COLUMNS );
            using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
            type & field = std::get<std::tuple_size<base>::value - N> ( S::COLUMNS );
            using fieldtype = typename std::remove_pointer<type>::type;
            field[l_number] = fieldtype::castString ( token.c_str(), token.size() );
            //Loader<S, std::tuple_size<base>::value - N>::addkey(field[l_number], l_number);
            //Loader<S, std::tuple_size<base>::value - N>::addkey(field[l_number], l_number);
        }
        else
        {
            using base = decltype ( S::TUPLE );
            using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
            type & field = std::get<std::tuple_size<base>::value - N> ( S::relation[l_number].TUPLE );
            field = type::castString ( token.c_str(), token.size() );
        }
        Loader<S, N - 1>::crazyTemplateLoader ( ss, line, l_number );
    }

    /*static void addkey ( auto key, uint64_t l_number )
    {

    }*/

    static void crazyPrint ( uint64_t l_number )
    {
        if ( S::columnstore )
        {
            using base = decltype ( S::COLUMNS );
            using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
            type & field = std::get<std::tuple_size<base>::value - N> ( S::COLUMNS );
            std::cout << field[l_number] << " ";
        }
        else
        {

            S t = S::relation[l_number];
            using base = decltype ( S::TUPLE );
            using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
            type & field = std::get<std::tuple_size<base>::value - N> ( t.TUPLE );
            std::cout << field << " ";
        }
        Loader<S, N - 1>::crazyPrint ( l_number );

    }

    static void columninit ( uint64_t size )
    {
        using base = decltype ( S::COLUMNS );
        using type = typename std::tuple_element<std::tuple_size<base>::value - N,base>::type;
        type & field = std::get<std::tuple_size<base>::value - N> ( S::COLUMNS );
        using fieldtype = typename std::remove_pointer<type>::type;
        field = new fieldtype[size];
        Loader<S, N - 1>::columninit ( size );
    }

};
template<typename S>
struct Loader<S, 0>
{

    /*static void addkey ( auto key, uint64_t l_number )
    {
        using base = decltype(S::COLUMNS);
        using p_type = typename std::tuple_element<0,base>::type;
        using type = typename std::remove_pointer<p_type>::type;

        S::primary_index.insert ( std::pair<type, uint64_t> ( std::get<S::primary_key> ( S::COLUMNS ) [l_number], l_number ) );
    }*/


    static void crazyTemplateLoader ( istringstream & ss,  string line, uint64_t l_number )
    {
        // do nothing -> recursion end
    }

    static void crazyPrint ( uint64_t l_number )
    {
        std::cout <<  endl;
    }

    static void columninit ( uint64_t size )
    {

    }
};

template<typename r>
struct relation
{

    static void print()
    {
        for ( uint64_t i = 0; i < r::count; i++ )
        {

            Loader<r, std::tuple_size<decltype ( r::TUPLE ) >::value>::crazyPrint ( i );
        }
    }

    static void load_relation()
    {
        string line;
        ifstream myfile;

        myfile.open ( "./../tbl/tpcc_" + string ( r::name ) + ".tbl" );
        r::count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
        myfile.clear();
        myfile.seekg ( 0 );

        if ( r::columnstore )
        {
            Loader<r, std::tuple_size<decltype ( r::TUPLE ) >::value>::columninit ( r::count );
        }
        else
        {
            r::relation = new r[r::count];
        }

        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                Loader<r, std::tuple_size<decltype ( r::TUPLE ) >::value>::crazyTemplateLoader ( ss, line, l_number );
                l_number++;
            }
            myfile.close();
        }
    }




};


static void free_relations()
{
    delete item::relation;
    delete warehouse::relation;
    delete district::relation;
    delete customer::relation;
    delete history::relation;
    delete neworder::relation;
    delete order::relation;
    delete orderline::relation;
    delete stock::relation;
}


# endif


uint64_t item_count = 0;
uint64_t warehouse_count = 0;
uint64_t district_count = 0;
uint64_t customer_count = 0;
uint64_t history_count = 0;
uint64_t neworder_count = 0;
uint64_t order_count = 0;
uint64_t orderline_count = 0;
uint64_t stock_count = 0;

item* item_relation = NULL;
warehouse* warehouse_relation = NULL;
district* district_relation = NULL;
customer* customer_relation = NULL;
history* history_relation = NULL;
neworder* neworder_relation = NULL;
order* order_relation = NULL;
orderline* orderline_relation = NULL;
stock* stock_relation = NULL;

item_columns item_relation_c;
warehouse_columns warehouse_relation_c;
district_columns district_relation_c;
customer_columns customer_relation_c;
history_columns history_relation_c;
neworder_columns neworder_relation_c;
order_columns order_relation_c;
orderline_columns orderline_relation_c;
stock_columns stock_relation_c;

string item_name = "item";
string warehouse_name = "warehouse";
string district_name = "district";
string customer_name = "customer";
string history_name = "history";
string neworder_name = "neworder";
string order_name = "order";
string orderline_name = "orderline";
string stock_name = "stock";

bool item_columnstore = true;
bool warehouse_columnstore = false;
bool district_columnstore = false;
bool customer_columnstore = false;
bool history_columnstore = false;
bool neworder_columnstore = false;
bool order_columnstore = false;
bool orderline_columnstore = false;
bool stock_columnstore = false;

void print_item()
{
    if ( item_columnstore )
    {
        for ( int i = 0; i < item_count; i++ )
        {
            std::cout << item_relation_c.i_id[i] << ' '
                      << item_relation_c.i_im_id[i] << ' '<< item_relation_c.i_name[i] << ' '<< item_relation_c.i_price[i]
                      << ' '<< item_relation_c.i_data[i] << std::endl;
        }
    }
    else
    {

        for ( int i = 0; i < item_count; i++ )
        {
            std::cout << item_relation[i].i_id << ' '<< item_relation[i].i_im_id << ' '<< item_relation[i].i_name << ' '<< item_relation[i].i_price << ' '<< item_relation[i].i_data << std::endl;
        }
    }

}



void print_warehouse()
{
    if ( item_columnstore )
    {
        for ( int i = 0; i < item_count; i++ )
        {

            std::cout << warehouse_relation_c.w_id[i] << ' '
                      << warehouse_relation_c.w_id[i] << ' '<< warehouse_relation_c.w_street_1[i] << ' '<< warehouse_relation_c.w_street_2[i]
                      << ' '<< warehouse_relation_c.w_city[i] << ' '<< warehouse_relation_c.w_state[i] << ' '<< warehouse_relation_c.w_zip[i] <<
                      ' '<< warehouse_relation_c.w_tax[i] << ' '<< warehouse_relation_c.w_ytd[i] << std::endl;
        }
    }
    else
    {

        for ( int i = 0; i < item_count; i++ )
        {

            std::cout << warehouse_relation[i].w_id << ' '
                      << warehouse_relation[i].w_id << ' '<< warehouse_relation[i].w_street_1 << ' '<< warehouse_relation[i].w_street_2
                      << ' '<< warehouse_relation[i].w_city << ' '<< warehouse_relation[i].w_state << ' '<< warehouse_relation[i].w_zip <<
                      ' '<< warehouse_relation[i].w_tax << ' '<< warehouse_relation[i].w_ytd << std::endl;
        }
    }

}


static void load_item()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( item_name ) + ".tbl" );
    item_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( item_columnstore )
    {
        item_relation_c.i_id = new Integer[item_count];
        item_relation_c.i_im_id = new Integer[item_count];
        item_relation_c.i_name = new Varchar<24>[item_count];
        item_relation_c.i_price = new Numeric<5,2>[item_count];
        item_relation_c.i_data = new Varchar<50>[item_count];



        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                item_relation_c.i_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation_c.i_im_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation_c.i_name[l_number] = Varchar<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation_c.i_price[l_number] = Numeric<5,2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation_c.i_data[l_number] = Varchar<50>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        item_relation = new item[item_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                item_relation[l_number].i_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation[l_number].i_im_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation[l_number].i_name = Varchar<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation[l_number].i_price = Numeric<5,2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                item_relation[l_number].i_data = Varchar<50>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_warehouse()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( item_name ) + ".tbl" );
    warehouse_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( warehouse_columnstore )
    {
        warehouse_relation_c.w_id = new Integer[warehouse_count];
        warehouse_relation_c.w_name = new Varchar<10>[warehouse_count];
        warehouse_relation_c.w_street_1 = new Varchar<20>[warehouse_count];
        warehouse_relation_c.w_street_2 = new Varchar<20>[warehouse_count];
        warehouse_relation_c.w_city = new Varchar<20>[warehouse_count];
        warehouse_relation_c.w_state = new Char<2>[warehouse_count];
        warehouse_relation_c.w_zip = new Char<9>[warehouse_count];
        warehouse_relation_c.w_tax = new Numeric<4, 4>[warehouse_count];
        warehouse_relation_c.w_ytd = new Numeric<4, 4>[warehouse_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                warehouse_relation_c.w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_name[l_number] = Varchar<10>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_street_1[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_street_2[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_city[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_state[l_number] = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_zip[l_number] = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_tax[l_number] = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation_c.w_ytd[l_number] = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        warehouse_relation = new warehouse[warehouse_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_name = Varchar<10>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_city = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_state = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_zip = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_tax = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                warehouse_relation[l_number].w_ytd = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_district()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( item_name ) + ".tbl" );
    district_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( district_columnstore )
    {
        district_relation_c.d_id = new Integer[district_count];
        district_relation_c.d_w_id = new Integer[district_count];
        district_relation_c.d_name = new Varchar<10>[district_count];
        district_relation_c.d_street_1 = new Varchar<20>[district_count];
        district_relation_c.d_street_2 = new Varchar<20>[district_count];
        district_relation_c.d_city = new Varchar<20>[district_count];
        district_relation_c.d_state = new Char<2>[district_count];
        district_relation_c.d_zip = new Char<9>[district_count];
        district_relation_c.d_tax = new Numeric<4, 4>[district_count];
        district_relation_c.d_ytd = new Numeric<12, 2>[district_count];
        district_relation_c.d_next_o_id = new Integer[district_count];



        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                district_relation_c.d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_name[l_number] = Varchar<10>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_street_1[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_street_2[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_city[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_state[l_number] = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_zip[l_number] = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_tax[l_number] = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_ytd[l_number] = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation_c.d_next_o_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        district_relation = new district[district_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                district_relation[l_number].d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_name = Varchar<10>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_city = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_state = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_zip = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_tax = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_ytd = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                district_relation[l_number].d_next_o_id = Integer::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_customer()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( item_name ) + ".tbl" );
    customer_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( customer_columnstore )
    {
        customer_relation_c.c_id = new Integer[customer_count];
        customer_relation_c.c_d_id = new Integer[customer_count];
        customer_relation_c.c_w_id = new Integer[customer_count];
        customer_relation_c.c_first = new Varchar<16>[customer_count];
        customer_relation_c.c_middle = new Char<2>[customer_count];
        customer_relation_c.c_last = new Varchar<16>[customer_count];
        customer_relation_c.c_street_1 = new Varchar<20>[customer_count];
        customer_relation_c.c_street_2 = new Varchar<20>[customer_count];
        customer_relation_c.c_city = new Varchar<20>[customer_count];
        customer_relation_c.c_state = new Char<2>[customer_count];
        customer_relation_c.c_zip = new Char<9>[customer_count];
        customer_relation_c.c_phone = new Char<16>[customer_count];
        customer_relation_c.c_since = new Timestamp[customer_count];
        customer_relation_c.c_credit = new Char<2>[customer_count];
        customer_relation_c.c_credit_lim = new Numeric<12, 2>[customer_count];
        customer_relation_c.c_discount  = new Numeric<4, 4>[customer_count];
        customer_relation_c.c_balance = new Numeric<12, 2>[customer_count];
        customer_relation_c.c_ytd_paymenr = new Numeric<12, 2>[customer_count];
        customer_relation_c.c_payment_cnt = new Numeric<4, 0>[customer_count];
        customer_relation_c.c_delivery_cnt = new Numeric<4, 0>[customer_count];
        customer_relation_c.c_data = new Varchar<500>[customer_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                customer_relation_c.c_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_first[l_number] = Varchar<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_middle[l_number] = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_last[l_number] = Varchar<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_street_1[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_street_2[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_city[l_number] = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_state[l_number] = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_zip[l_number] = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_phone[l_number] = Char<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_since[l_number] = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_credit[l_number] = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_credit_lim[l_number] = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_discount [l_number] = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_balance[l_number] = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_ytd_paymenr[l_number] = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_payment_cnt[l_number] = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_delivery_cnt[l_number] = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation_c.c_data[l_number] = Varchar<500>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );

                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        customer_relation = new customer[customer_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                customer_relation[l_number].c_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_first = Varchar<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_middle = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_last = Varchar<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_city = Varchar<20>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_state = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_zip = Char<9>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_phone = Char<16>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_since = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_credit = Char<2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_credit_lim = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_discount  = Numeric<4, 4>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_balance = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_ytd_paymenr = Numeric<12, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_payment_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_delivery_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                customer_relation[l_number].c_data = Varchar<500>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_history()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( history_name ) + ".tbl" );
    history_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( history_columnstore )
    {
        history_relation_c.h_c_id = new Integer[history_count];
        history_relation_c.h_c_id = new Integer[history_count];
        history_relation_c.h_c_w_id = new Integer[history_count];
        history_relation_c.h_d_id = new Integer[history_count];
        history_relation_c.h_w_id = new Integer[history_count];
        history_relation_c.h_date = new Timestamp[history_count];
        history_relation_c.h_amount = new Numeric<6,2>[history_count];
        history_relation_c.h_data = new Varchar<24>[history_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                history_relation_c.h_c_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_c_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_c_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_date[l_number] = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_amount[l_number] = Numeric<6,2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation_c.h_data[l_number] = Varchar<24>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        history_relation = new history[history_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                history_relation[l_number].h_c_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_c_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_c_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_date = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_amount = Numeric<6,2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                history_relation[l_number].h_data = Varchar<24>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_neworder()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( neworder_name ) + ".tbl" );
    neworder_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( neworder_columnstore )
    {
        neworder_relation_c.no_o_id = new Integer[neworder_count];
        neworder_relation_c.no_d_id = new Integer[neworder_count];
        neworder_relation_c.no_w_id = new Integer[neworder_count];

        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                neworder_relation_c.no_o_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                neworder_relation_c.no_d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                neworder_relation_c.no_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        neworder_relation = new neworder[neworder_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                neworder_relation[l_number].no_o_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                neworder_relation[l_number].no_d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                neworder_relation[l_number].no_w_id = Integer::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_order()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( order_name ) + ".tbl" );
    order_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( order_columnstore )
    {
        order_relation_c.o_id = new Integer[order_count];
        order_relation_c.o_d_id = new Integer[order_count];
        order_relation_c.o_w_id = new Integer[order_count];
        order_relation_c.o_c_id = new Integer[order_count];
        order_relation_c.o_entry_d = new Timestamp[order_count];
        order_relation_c.o_carrier_id = new Integer[order_count];
        order_relation_c.o_ol_cnt = new Numeric<2,0>[order_count];
        order_relation_c.o_all_local = new Numeric<1,0>[order_count];

        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                order_relation_c.o_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_c_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_entry_d[l_number] = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_carrier_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_ol_cnt[l_number] = Numeric<2,0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation_c.o_all_local[l_number] = Numeric<1,0>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        order_relation = new order[order_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                order_relation[l_number].o_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_c_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_entry_d = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_carrier_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_ol_cnt = Numeric<2,0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                order_relation[l_number].o_all_local = Numeric<1,0>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


static void load_orderline()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( orderline_name ) + ".tbl" );
    orderline_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( orderline_columnstore )
    {
        orderline_relation_c.ol_o_id = new Integer[orderline_count];
        orderline_relation_c.ol_d_id = new Integer[orderline_count];
        orderline_relation_c.ol_w_id = new Integer[orderline_count];
        orderline_relation_c.ol_number = new Integer[orderline_count];
        orderline_relation_c.ol_i_id = new Integer[orderline_count];
        orderline_relation_c.ol_supply_w_id = new Integer[orderline_count];
        orderline_relation_c.ol_delivery_d = new Timestamp[orderline_count];
        orderline_relation_c.ol_quantity = new Numeric<2, 0>[orderline_count];
        orderline_relation_c.ol_amount = new Numeric<6, 2>[orderline_count];
        orderline_relation_c.ol_dist_info = new Char<24>[orderline_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                orderline_relation_c.ol_o_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_d_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_number[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_i_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_supply_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_delivery_d[l_number] = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_quantity[l_number] = Numeric<2, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_amount[l_number] = Numeric<6, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation_c.ol_dist_info[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        orderline_relation = new orderline[orderline_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_o_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_d_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_number = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_i_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_supply_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_delivery_d = Timestamp::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_quantity = Numeric<2, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_amount = Numeric<6, 2>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                orderline_relation[l_number].ol_dist_info = Char<24>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}



static void load_stock()
{
    string line;
    ifstream myfile;

    myfile.open ( "./../tbl/tpcc_" + string ( stock_name ) + ".tbl" );
    stock_count = std::count ( std::istreambuf_iterator<char> ( myfile ),  std::istreambuf_iterator<char>(), '\n' );
    myfile.clear();
    myfile.seekg ( 0 );

    if ( stock_columnstore )
    {
        stock_relation_c.s_i_id = new Integer[stock_count];
        stock_relation_c.s_w_id = new Integer[stock_count];
        stock_relation_c.s_quantity = new Numeric<4, 0>[stock_count];
        stock_relation_c.s_dist_01 = new Char<24>[stock_count];
        stock_relation_c.s_dist_02 = new Char<24>[stock_count];
        stock_relation_c.s_dist_03 = new Char<24>[stock_count];
        stock_relation_c.s_dist_04 = new Char<24>[stock_count];
        stock_relation_c.s_dist_05 = new Char<24>[stock_count];
        stock_relation_c.s_dist_06 = new Char<24>[stock_count];
        stock_relation_c.s_dist_07 = new Char<24>[stock_count];
        stock_relation_c.s_dist_08 = new Char<24>[stock_count];
        stock_relation_c.s_dist_09 = new Char<24>[stock_count];
        stock_relation_c.s_dist_10 = new Char<24>[stock_count];
        stock_relation_c.s_ytd = new Numeric<8, 0>[stock_count];
        stock_relation_c.s_order_cnt = new Numeric<4, 0>[stock_count];
        stock_relation_c.s_remote_cnt = new Numeric<4, 0>[stock_count];
        stock_relation_c.s_data = new Varchar<50>[stock_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                stock_relation_c.s_i_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_w_id[l_number] = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_quantity[l_number] = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_01[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_02[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_03[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_04[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_05[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_06[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_07[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_08[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_09[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_dist_10[l_number] = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_ytd[l_number] = Numeric<8, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_order_cnt[l_number] = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_remote_cnt[l_number] = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation_c.s_data[l_number] = Varchar<50>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
    else
    {
        stock_relation = new stock[stock_count];


        if ( myfile.is_open() )
        {
            uint64_t l_number = 0;
            while ( getline ( myfile,line ) )
            {
                istringstream ss ( line );
                string token;
                getline ( ss, token, '|' );
                stock_relation[l_number].s_i_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_w_id = Integer::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_quantity = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_01 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_02 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_03 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_04 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_05 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_06 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_07 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_08 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_09 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_dist_10 = Char<24>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_ytd = Numeric<8, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_order_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_remote_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
                getline ( ss, token, '|' );
                stock_relation[l_number].s_data = Varchar<50>::castString ( token.c_str(), token.size() );
                l_number++;
            }
            myfile.close();
        }
    }
}


int main ( int argc, char **argv )
{


    load_item();
    print_item();

    /*relation<item>::load_relation();
    cout << item::primary_index[50] << endl;
    relation<item>::print();
    relation<warehouse>::load_relation();
    relation<district>::load_relation();
    relation<history>::load_relation();
    relation<neworder>::load_relation();
    relation<order>::load_relation();
    relation<orderline>::load_relation();
    relation<stock>::load_relation();
    relation<district>::load_relation();
    relation<customer>::load_relation();
    */


    //free_relations();
    return 0;
}


