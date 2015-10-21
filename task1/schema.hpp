#include <iostream>
#include <tuple>
#include <string>
#include "Types.hpp"
#include <typeinfo>
#include <map>
#define USETUPLES 0
using namespace std;


#if 0

   /* template <typename T>
    struct table
    {
        T TUPLE
        static constexpr bool columnstore = true;
        static constexpr char const *name = typeid(T).name();
        static uint64_t count;
        static item * relation;
        
    };*/


    struct item
    {
        typedef std::tuple<Integer, Integer,  Varchar<24>,  Numeric<5,2>,  Varchar<50>> t_type;
        t_type TUPLE;
        typedef std::tuple<Integer*, Integer*,  Varchar<24>*,  Numeric<5,2>*,  Varchar<50>*> p_type;
        
        static std::tuple<Integer*, Integer*,  Varchar<24>*,  Numeric<5,2>*,  Varchar<50>*> COLUMNS;
        static constexpr bool columnstore = true;
        static constexpr char const *name = "item";
        static uint64_t count;
        static item * relation;
        
        
        //typedef std::tuple_element<primary_key,t_type>::type primary_type;
        //static std::map<std::tuple_element<primary_key,t_type>::type,uint64_t> primary_index;
        static constexpr unsigned primary_key_count = 1;
        static constexpr unsigned primary_key[] = {0};
        typedef std::tuple<Integer> primary_key_type;
        static std::map<primary_key_type,uint64_t> primary_index;
        
        static void insert(primary_key_type key, uint64_t l_number)
        {
            primary_index.insert(std::pair<primary_key_type, uint64_t>(key, l_number));
        }
        
        static void make_index()
        {
            if (columnstore)
            {
                for (int i = 0; i < count; i++)
                {
                    
                        primary_key_type k = make_tuple(get<0>(COLUMNS)[i]);
                }
            }else{
                
            }
            
        }
        
        Integer& i_id(){ return std::get<0>(TUPLE);}
        Integer& i_im_id(){ return std::get<1>(TUPLE);}
        Varchar<24>& i_name(){ return std::get<2>(TUPLE);}
        Numeric<5,2>& i_price(){ return std::get<3>(TUPLE);}
        Varchar<50>& i_data(){ return std::get<4>(TUPLE);}

    };
    
    /*
    template <unsigned N> struct item_index{ static void addkey ( auto key, uint64_t l_number ){} };
    
    template <> struct item_index<item::primary_key> {
        static void addkey ( auto key, uint64_t l_number )
        {
            using base = decltype(item::COLUMNS);
            using p_type = typename std::tuple_element<item::primary_index,base>::type;
            using type = typename std::remove_pointer<p_type>::type;
            
            item::primary_index.insert ( std::pair<type, uint64_t> ( std::get<S::primary_key> ( item::COLUMNS ) [l_number], l_number ) );
        }
    };*/
    
   
    
  
    
    
    struct warehouse
    {

        std::tuple<Integer, Varchar<10>, Varchar<20>, Varchar<20>, Varchar<20>, 
            Char<2>, Char<9>, Numeric<4, 4>, Numeric<4, 4>> TUPLE;
        
        static std::tuple<Integer*, Varchar<10>*, Varchar<20>*, Varchar<20>*, Varchar<20>*, 
            Char<2>*, Char<9>*, Numeric<4, 4>*, Numeric<4, 4>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "warehouse";
        static uint64_t count;
        static warehouse * relation;
        
       
        Integer& w_id(){ return std::get<0>(TUPLE);}
        Varchar<10>& w_name(){ return std::get<1>(TUPLE);}
        Varchar<20>& w_street_1(){ return std::get<2>(TUPLE);}
        Varchar<20>& w_street_2(){ return std::get<3>(TUPLE);}
        Varchar<20>& w_city(){ return std::get<4>(TUPLE);}
        Char<2>& w_state(){ return std::get<5>(TUPLE);}
        Char<9>& w_zip(){ return std::get<6>(TUPLE);}
        Numeric<4, 4>& w_tax(){ return std::get<7>(TUPLE);}
        Numeric<4, 4>& w_ytd(){ return std::get<8>(TUPLE);}
   
    };
    
    struct district
    {
        
        std::tuple<Integer, Integer, Varchar<10>, Varchar<20>, Varchar<20>, Varchar<20>, 
            Char<2>, Char<9>, Numeric<4, 4>, Numeric<12, 2>, Integer> TUPLE;
            
        static std::tuple<Integer*, Integer*, Varchar<10>*, Varchar<20>*, Varchar<20>*, Varchar<20>*, 
            Char<2>*, Char<9>*, Numeric<4, 4>*, Numeric<12, 2>*, Integer*> COLUMNS;    
        static constexpr bool columnstore = false;
        static constexpr char const *name = "district";
        static uint64_t count;
        static district * relation;
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& d_id(){ return std::get<0>(TUPLE);}
        Integer& d_w_id(){ return std::get<1>(TUPLE);}
        Varchar<10>& d_name(){ return std::get<2>(TUPLE);}
        Varchar<20>& d_street_1(){ return std::get<3>(TUPLE);}
        Varchar<20>& d_street_2(){ return std::get<4>(TUPLE);}
        Varchar<20>& d_city(){ return std::get<5>(TUPLE);}
        Char<2>& d_state(){ return std::get<6>(TUPLE);}
        Char<9>& d_zip(){ return std::get<7>(TUPLE);}
        Numeric<4,4>& d_tax(){ return std::get<8>(TUPLE);}
        Numeric<12,2>& d_ytd(){ return std::get<9>(TUPLE);}
        Integer& d_next_o_id(){ return std::get<10>(TUPLE);}
    };

    struct customer
    {
        std::tuple<Integer, Integer, Integer, Varchar<16>, Char<2>, Varchar<16>, Varchar<20>, Varchar<20>, Varchar<20>, 
            Char<2>, Char<9>, Char<16>, Timestamp, Char<2>, Numeric<12, 2>, Numeric<4, 4>, Numeric<12, 2>, Numeric<12, 2>,
            Numeric<4, 0>, Numeric<4, 0>, Varchar<500>> TUPLE;
        
        static std::tuple<Integer*, Integer*, Integer*, Varchar<16>*, Char<2>*, Varchar<16>*, Varchar<20>*, Varchar<20>*, Varchar<20>*, 
            Char<2>*, Char<9>*, Char<16>*, Timestamp*, Char<2>*, Numeric<12, 2>*, Numeric<4, 4>*, Numeric<12, 2>*, Numeric<12, 2>*,
            Numeric<4, 0>*, Numeric<4, 0>*, Varchar<500>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "customer";
        static uint64_t count;
        static customer * relation;
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        
        Integer& c_id(){ return std::get<0>(TUPLE);}
        Integer& c_d_id(){ return std::get<1>(TUPLE);}
        Integer& c_w_id(){ return std::get<2>(TUPLE);}
        Varchar<16>& c_first(){ return std::get<3>(TUPLE);}
        Char<2>& c_middle(){ return std::get<4>(TUPLE);}
        Varchar<16>& c_last(){ return std::get<5>(TUPLE);}
        Varchar<20>& c_street_1(){ return std::get<6>(TUPLE);}
        Varchar<20>& c_street_2(){ return std::get<7>(TUPLE);}
        Varchar<20>& c_city(){ return std::get<8>(TUPLE);}
        Char<2>& c_state(){ return std::get<9>(TUPLE);}
        Char<9>& c_zip(){ return std::get<10>(TUPLE);}
        Char<16>& c_phone(){ return std::get<11>(TUPLE);}
        Timestamp& c_since(){ return std::get<12>(TUPLE);}
        Char<2>& c_credit(){ return std::get<13>(TUPLE);}
        Numeric<12,2>& c_credit_lim(){ return std::get<14>(TUPLE);}
        Numeric<4,4>& c_discount(){ return std::get<15>(TUPLE);}
        Numeric<12,2>& c_balance(){ return std::get<16>(TUPLE);}
        Numeric<12,2>& c_ytd_paymenr(){ return std::get<17>(TUPLE);}
        Numeric<4,0>& c_payment_cnt(){ return std::get<18>(TUPLE);}
        Numeric<4,0>& c_delivery_cnt(){ return std::get<19>(TUPLE);}
        Varchar<500>& c_data(){ return std::get<20>(TUPLE);}
    };

    struct history
    {
        std::tuple<Integer, Integer, Integer, Integer, Integer, Timestamp, Numeric<6, 2>, Varchar<24>> TUPLE;
        
        static std::tuple<Integer*, Integer*, Integer*, Integer*, Integer*, Timestamp*, Numeric<6, 2>*, Varchar<24>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "history";
        static uint64_t count;
        static history * relation;
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& h_c_id(){ return std::get<0>(TUPLE);}
        Integer& h_c_d_id(){ return std::get<1>(TUPLE);}
        Integer& h_c_w_id(){ return std::get<2>(TUPLE);}
        Integer& h_d_id(){ return std::get<3>(TUPLE);}
        Integer& h_w_id(){ return std::get<4>(TUPLE);}
        Timestamp& h_date(){ return std::get<5>(TUPLE);}
        Numeric<6,2>& h_amount(){ return std::get<6>(TUPLE);}
        Varchar<24>& h_data(){ return std::get<7>(TUPLE);}
    };

    struct neworder
    {
        std::tuple<Integer, Integer, Integer> TUPLE;
        
        static std::tuple<Integer*, Integer*, Integer*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "neworder";
        static uint64_t count;
        static neworder * relation;
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& no_o_id(){ return std::get<0>(TUPLE);}
        Integer& no_d_id(){ return std::get<1>(TUPLE);}
        Integer& no_w_id(){ return std::get<2>(TUPLE);}
    };

    struct order
    {
        std::tuple<Integer, Integer, Integer, Integer, Timestamp, Integer, Numeric<2, 0>, Numeric<1, 0>> TUPLE;
        
        static std::tuple<Integer*, Integer*, Integer*, Integer*, Timestamp*, Integer*, Numeric<2, 0>*, Numeric<1, 0>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "order";
        static uint64_t count;
        static order * relation;
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& o_id(){ return std::get<0>(TUPLE);}
        Integer& o_d_id(){ return std::get<1>(TUPLE);}
        Integer& o_w_id(){ return std::get<2>(TUPLE);}
        Integer& o_c_id(){ return std::get<3>(TUPLE);}
        Timestamp& o_entry_d(){ return std::get<4>(TUPLE);}
        Integer& o_carrier_id(){ return std::get<5>(TUPLE);}
        Numeric<2,0>& o_ol_cnt(){ return std::get<6>(TUPLE);}
        Numeric<1,0>& o_all_local(){ return std::get<7>(TUPLE);}
    };

    struct orderline
    {
        std::tuple<Integer, Integer, Integer, Integer, Integer, Integer,Timestamp, Numeric<2, 0>, Numeric<6, 2>, Char<24>> TUPLE;
        
        static std::tuple<Integer*, Integer*, Integer*, Integer*, Integer*, Integer*,Timestamp*, Numeric<2, 0>*, Numeric<6, 2>*, Char<24>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "orderline";
        static uint64_t count;
        static orderline * relation;       
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& ol_o_id(){ return std::get<0>(TUPLE);}
        Integer& ol_d_id(){ return std::get<1>(TUPLE);}
        Integer& ol_w_id(){ return std::get<2>(TUPLE);}
        Integer& ol_number(){ return std::get<3>(TUPLE);}
        Integer& ol_i_id(){ return std::get<4>(TUPLE);}
        Integer& ol_supply_w_id(){ return std::get<5>(TUPLE);}
        Timestamp& ol_delivery_d(){ return std::get<6>(TUPLE);}
        Numeric<2,0>& ol_quantity(){ return std::get<7>(TUPLE);}
        Numeric<6,2>& ol_amount(){ return std::get<8>(TUPLE);}
        Char<24>& ol_dist_info(){ return std::get<9>(TUPLE);}
    };



    struct stock
    {
        std::tuple<Integer, Integer, Numeric<4, 0>, Char<24>, Char<24>, Char<24>, 
        Char<24>, Char<24>, Char<24>, Char<24>, Char<24>, Char<24>, Char<24>, 
        Numeric<8, 0>, Numeric<4, 0>,Numeric<4, 0>,Varchar<50> > TUPLE;
        
        static std::tuple<Integer*, Integer*, Numeric<4, 0>*, Char<24>*, Char<24>*, Char<24>*, 
        Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*, Char<24>*, 
        Numeric<8, 0>*, Numeric<4, 0>*,Numeric<4, 0>*,Varchar<50>*> COLUMNS;
        static constexpr bool columnstore = false;
        static constexpr char const *name = "stock";
        static uint64_t count;
        static stock * relation; 
        template <unsigned N> static void addkey ( auto key, uint64_t l_number ){}
        
        Integer& s_i_id(){ return std::get<0>(TUPLE);}
        Integer& s_w_id(){ return std::get<1>(TUPLE);}
        Numeric<4,0>& s_quantity(){ return std::get<2>(TUPLE);}
        Char<24>& s_dist_01(){ return std::get<3>(TUPLE);}
        Char<24>& s_dist_02(){ return std::get<4>(TUPLE);}
        Char<24>& s_dist_03(){ return std::get<5>(TUPLE);}
        Char<24>& s_dist_04(){ return std::get<6>(TUPLE);}
        Char<24>& s_dist_05(){ return std::get<7>(TUPLE);}
        Char<24>& s_dist_06(){ return std::get<8>(TUPLE);}
        Char<24>& s_dist_07(){ return std::get<9>(TUPLE);}
        Char<24>& s_dist_08(){ return std::get<10>(TUPLE);}
        Char<24>& s_dist_09(){ return std::get<11>(TUPLE);}
        Char<24>& s_dist_10(){ return std::get<12>(TUPLE);}
        Numeric<8,0>& s_ytd(){ return std::get<13>(TUPLE);}
        Numeric<4,0>& s_order_cnt(){ return std::get<14>(TUPLE);}
        Numeric<4,0>& s_remote_cnt(){ return std::get<15>(TUPLE);}
        Varchar<50>& s_data(){ return std::get<16>(TUPLE);}
    };

    
# else
    struct item
    {
        Integer i_id;
        Integer i_im_id;
        Varchar<24> i_name;
        Numeric<5,2> i_price;
        Varchar<50> i_data;

     };
     
    struct item_columns
    {
        Integer* i_id;
        Integer* i_im_id;
        Varchar<24>* i_name;
        Numeric<5,2>* i_price;
        Varchar<50>* i_data;
       
    };
     
     

    struct warehouse
    {
        Integer w_id;
        Varchar<10> w_name;
        Varchar<20> w_street_1;
        Varchar<20> w_street_2;
        Varchar<20> w_city;
        Char<2> w_state;
        Char<9> w_zip;
        Numeric<4, 4> w_tax;
        Numeric<12, 2> w_ytd;

    };

    
    struct warehouse_columns
    {
        Integer* w_id;
        Varchar<10>* w_name;
        Varchar<20>* w_street_1;
        Varchar<20>* w_street_2;
        Varchar<20>* w_city;
        Char<2>* w_state;
        Char<9>* w_zip;
        Numeric<4, 4>* w_tax;
        Numeric<12, 2>* w_ytd;

    };
    
    struct district
    {
        Integer d_id;
        Integer d_w_id;
        Varchar<10> d_name;
        Varchar<20> d_street_1;
        Varchar<20> d_street_2;
        Varchar<20> d_city;
        Char<2> d_state;
        Char<9> d_zip;
        Numeric<4,4> d_tax;
        Numeric<12,2> d_ytd;
        Integer d_next_o_id;
    };
    
        
    struct district_columns
    {
        Integer* d_id;
        Integer* d_w_id;
        Varchar<10>* d_name;
        Varchar<20>* d_street_1;
        Varchar<20>* d_street_2;
        Varchar<20>* d_city;
        Char<2>* d_state;
        Char<9>* d_zip;
        Numeric<4,4>* d_tax;
        Numeric<12,2>* d_ytd;
        Integer* d_next_o_id;
    };

    struct customer
    {
        Integer c_id;
        Integer c_d_id;
        Integer c_w_id;
        Varchar<16> c_first;
        Char<2> c_middle;
        Varchar<16> c_last;
        Varchar<20> c_street_1;
        Varchar<20> c_street_2;
        Varchar<20> c_city;
        Char<2> c_state;
        Char<9> c_zip;
        Char<16> c_phone;
        Timestamp c_since;
        Char<2> c_credit;
        Numeric<12,2> c_credit_lim;
        Numeric<4,4> c_discount;
        Numeric<12,2> c_balance;
        Numeric<12,2> c_ytd_paymenr;
        Numeric<4,0> c_payment_cnt;
        Numeric<4,0> c_delivery_cnt;
        Varchar<500> c_data;
    };
    
    
    struct customer_columns
    {
        Integer* c_id;
        Integer* c_d_id;
        Integer* c_w_id;
        Varchar<16>* c_first;
        Char<2>* c_middle;
        Varchar<16>* c_last;
        Varchar<20>* c_street_1;
        Varchar<20>* c_street_2;
        Varchar<20>* c_city;
        Char<2>* c_state;
        Char<9>* c_zip;
        Char<16>* c_phone;
        Timestamp* c_since;
        Char<2>* c_credit;
        Numeric<12,2>* c_credit_lim;
        Numeric<4,4>* c_discount;
        Numeric<12,2>* c_balance;
        Numeric<12,2>* c_ytd_paymenr;
        Numeric<4,0>* c_payment_cnt;
        Numeric<4,0>* c_delivery_cnt;
        Varchar<500>* c_data;
    };

    struct history
    {
        Integer h_c_id;
        Integer h_c_d_id;
        Integer h_c_w_id;
        Integer h_d_id;
        Integer h_w_id;
        Timestamp h_date;
        Numeric<6,2> h_amount;
        Varchar<24> h_data;
    };

    
    struct history_columns
    {
        Integer* h_c_id;
        Integer* h_c_d_id;
        Integer* h_c_w_id;
        Integer* h_d_id;
        Integer* h_w_id;
        Timestamp* h_date;
        Numeric<6,2>* h_amount;
        Varchar<24>* h_data;
    };
    
    
    struct neworder
    {
        Integer no_o_id;
        Integer no_d_id;
        Integer no_w_id;
    };
    
    struct neworder_columns
    {
        Integer* no_o_id;
        Integer* no_d_id;
        Integer* no_w_id;
    };

    struct order
    {
        Integer o_id;
        Integer o_d_id;
        Integer o_w_id;
        Integer o_c_id;
        Timestamp o_entry_d;
        Integer o_carrier_id;
        Numeric<2,0> o_ol_cnt;
        Numeric<1,0> o_all_local;
    };
    
    struct order_columns
    {
        Integer* o_id;
        Integer* o_d_id;
        Integer* o_w_id;
        Integer* o_c_id;
        Timestamp* o_entry_d;
        Integer* o_carrier_id;
        Numeric<2,0>* o_ol_cnt;
        Numeric<1,0>* o_all_local;
    };

    struct orderline
    {
        Integer ol_o_id;
        Integer ol_d_id;
        Integer ol_w_id;
        Integer ol_number;
        Integer ol_i_id;
        Integer ol_supply_w_id;
        Timestamp ol_delivery_d;
        Numeric<2,0> ol_quantity;
        Numeric<6,2> ol_amount;
        Char<24> ol_dist_info;
    };

    struct orderline_columns
    {
        Integer* ol_o_id;
        Integer* ol_d_id;
        Integer* ol_w_id;
        Integer* ol_number;
        Integer* ol_i_id;
        Integer* ol_supply_w_id;
        Timestamp* ol_delivery_d;
        Numeric<2,0>* ol_quantity;
        Numeric<6,2>* ol_amount;
        Char<24>* ol_dist_info;
    };


    struct stock
    {
        Integer s_i_id;
        Integer s_w_id;
        Numeric<4,0> s_quantity;
        Char<24> s_dist_01;
        Char<24> s_dist_02;
        Char<24> s_dist_03;
        Char<24> s_dist_04;
        Char<24> s_dist_05;
        Char<24> s_dist_06;
        Char<24> s_dist_07;
        Char<24> s_dist_08;
        Char<24> s_dist_09;
        Char<24> s_dist_10;
        Numeric<8,0> s_ytd;
        Numeric<4,0> s_order_cnt;
        Numeric<4,0> s_remote_cnt;
        Varchar<50> s_data;
    };
    
    
    struct stock_columns
    {
        Integer* s_i_id;
        Integer* s_w_id;
        Numeric<4,0>* s_quantity;
        Char<24>* s_dist_01;
        Char<24>* s_dist_02;
        Char<24>* s_dist_03;
        Char<24>* s_dist_04;
        Char<24>* s_dist_05;
        Char<24>* s_dist_06;
        Char<24>* s_dist_07;
        Char<24>* s_dist_08;
        Char<24>* s_dist_09;
        Char<24>* s_dist_10;
        Numeric<8,0>* s_ytd;
        Numeric<4,0>* s_order_cnt;
        Numeric<4,0>* s_remote_cnt;
        Varchar<50>* s_data;
    };


# endif




