
#include "Types.hpp"


struct warehouse {
	Integer w_id;
	Varchar<10> w_name;
	Varchar<20> w_street_1;
	Varchar<20> w_street_2;
	Varchar<20> w_city;
	Char<2> w_state;
	Char<9> w_zip;
	Numeric<4, 4> w_tax;
	Numeric<4, 4> w_ytd;
};



struct district {
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

struct customer {
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

struct history {
	Integer h_c_id;
	Integer h_c_d_id;
	Integer h_c_w_id;
	Integer h_d_id;
	Integer h_w_id;
	Timestamp h_date;
	Numeric<6,2> h_amount;
	Varchar<24> h_data;
};

struct neworder {
	Integer no_o_id;
	Integer no_d_id;
	Integer no_w_id;
};

struct order {
	Integer o_id;
	Integer o_d_id;
	Integer o_w_id;
	Integer o_c_id;
	Timestamp o_entry_d;
	Integer o_carrier_id;
	Numeric<2,0> o_ol_cnt;
	Numeric<1,0> o_all_local;
};

struct orderline {
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

struct item {
	Integer i_id;
	Integer i_im_id;
	Varchar<24> i_name;
	Numeric<5,2> i_price;
	Varchar<50> i_data;
};

struct stock {
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
