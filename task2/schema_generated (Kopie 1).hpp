#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <exception>
#include "Types.hpp"
using namespace std;

struct warehouse{
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

struct district{
	Integer d_id;
	Integer d_w_id;
	Varchar<10> d_name;
	Varchar<20> d_street_1;
	Varchar<20> d_street_2;
	Varchar<20> d_city;
	Char<2> d_state;
	Char<9> d_zip;
	Numeric<4, 4> d_tax;
	Numeric<12, 2> d_ytd;
	Integer d_next_o_id;
};

struct customer{
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
	Numeric<12, 2> c_credit_lim;
	Numeric<4, 4> c_discount;
	Numeric<12, 2> c_balance;
	Numeric<12, 2> c_ytd_paymenr;
	Numeric<4, 0> c_payment_cnt;
	Numeric<4, 0> c_delivery_cnt;
	Varchar<500> c_data;
};

struct history{
	Integer h_c_id;
	Integer h_c_d_id;
	Integer h_c_w_id;
	Integer h_d_id;
	Integer h_w_id;
	Timestamp h_date;
	Numeric<6, 2> h_amount;
	Varchar<24> h_data;
};

struct neworder{
	Integer no_o_id;
	Integer no_d_id;
	Integer no_w_id;
};

struct order{
	Integer o_id;
	Integer o_d_id;
	Integer o_w_id;
	Integer o_c_id;
	Timestamp o_entry_d;
	Integer o_carrier_id;
	Numeric<2, 0> o_ol_cnt;
	Numeric<1, 0> o_all_local;
};

struct orderline{
	Integer ol_o_id;
	Integer ol_d_id;
	Integer ol_w_id;
	Integer ol_number;
	Integer ol_i_id;
	Integer ol_supply_w_id;
	Timestamp ol_delivery_d;
	Numeric<2, 0> ol_quantity;
	Numeric<6, 2> ol_amount;
	Char<24> ol_dist_info;
};

struct item{
	Integer i_id;
	Integer i_im_id;
	Varchar<24> i_name;
	Numeric<5, 2> i_price;
	Varchar<50> i_data;
};

struct stock{
	Integer s_i_id;
	Integer s_w_id;
	Numeric<4, 0> s_quantity;
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
	Numeric<8, 0> s_ytd;
	Numeric<4, 0> s_order_cnt;
	Numeric<4, 0> s_remote_cnt;
	Varchar<50> s_data;
};
string warehouse_name = "warehouse";
string district_name = "district";
string customer_name = "customer";
string history_name = "history";
string neworder_name = "neworder";
string order_name = "order";
string orderline_name = "orderline";
string item_name = "item";
string stock_name = "stock";

std::vector<warehouse> warehouse_row_relation;
std::vector<district> district_row_relation;
std::vector<customer> customer_row_relation;
std::vector<history> history_row_relation;
std::vector<neworder> neworder_row_relation;
std::vector<order> order_row_relation;
std::vector<orderline> orderline_row_relation;
std::vector<item> item_row_relation;
std::vector<stock> stock_row_relation;

std::map<Integer,std::map<Integer,std::map<Varchar<16>,std::map<Varchar<16>,uint64_t>>>> customer_wdl_index;
std::map<Integer,std::map<Integer,std::map<Integer,std::map<Integer,uint64_t>>>> order_wdc_index;
std::map<Integer,uint64_t> warehouse_primary_key;
std::map<Integer,std::map<Integer,uint64_t>> district_primary_key;
std::map<Integer,std::map<Integer,std::map<Integer,uint64_t>>> customer_primary_key;
std::map<Integer,std::map<Integer,std::map<Integer,uint64_t>>> neworder_primary_key;
std::map<Integer,std::map<Integer,std::map<Integer,uint64_t>>> order_primary_key;
std::map<Integer,std::map<Integer,std::map<Integer,std::map<Integer,uint64_t>>>> orderline_primary_key;
std::map<Integer,uint64_t> item_primary_key;
std::map<Integer,std::map<Integer,uint64_t>> stock_primary_key;
bool warehouse_insert(warehouse element){
	if (warehouse_primary_key.count(element.w_id)==1) {
		return false;
	}
	warehouse_row_relation.push_back(element);
	warehouse_primary_key[element.w_id] = warehouse_row_relation.size()-1;
	return true;
}
bool district_insert(district element){
	if (district_primary_key[element.d_w_id].count(element.d_id)==1) {
		return false;
	}
	district_row_relation.push_back(element);
	district_primary_key[element.d_w_id][element.d_id] = district_row_relation.size()-1;
	return true;
}
bool customer_insert(customer element){
	if (customer_primary_key[element.c_w_id][element.c_d_id].count(element.c_id)==1) {
		return false;
	}
	customer_row_relation.push_back(element);
	customer_primary_key[element.c_w_id][element.c_d_id][element.c_id] = customer_row_relation.size()-1;
	customer_wdl_index[element.c_w_id][element.c_d_id][element.c_last][element.c_first] = customer_row_relation.size();
	return true;
}
bool history_insert(history element){
	history_row_relation.push_back(element);
	return true;
}
bool neworder_insert(neworder element){
	if (neworder_primary_key[element.no_w_id][element.no_d_id].count(element.no_o_id)==1) {
		return false;
	}
	neworder_row_relation.push_back(element);
	neworder_primary_key[element.no_w_id][element.no_d_id][element.no_o_id] = neworder_row_relation.size()-1;
	return true;
}
bool order_insert(order element){
	if (order_primary_key[element.o_w_id][element.o_d_id].count(element.o_id)==1) {
		return false;
	}
	order_row_relation.push_back(element);
	order_primary_key[element.o_w_id][element.o_d_id][element.o_id] = order_row_relation.size()-1;
	order_wdc_index[element.o_w_id][element.o_d_id][element.o_c_id][element.o_id] = order_row_relation.size();
	return true;
}
bool orderline_insert(orderline element){
	if (orderline_primary_key[element.ol_w_id][element.ol_d_id][element.ol_o_id].count(element.ol_number)==1) {
		return false;
	}
	orderline_row_relation.push_back(element);
	orderline_primary_key[element.ol_w_id][element.ol_d_id][element.ol_o_id][element.ol_number] = orderline_row_relation.size()-1;
	return true;
}
bool item_insert(item element){
	if (item_primary_key.count(element.i_id)==1) {
		return false;
	}
	item_row_relation.push_back(element);
	item_primary_key[element.i_id] = item_row_relation.size()-1;
	return true;
}
bool stock_insert(stock element){
	if (stock_primary_key[element.s_w_id].count(element.s_i_id)==1) {
		return false;
	}
	stock_row_relation.push_back(element);
	stock_primary_key[element.s_w_id][element.s_i_id] = stock_row_relation.size()-1;
	return true;
}

bool warehouse_delete(uint64_t lineNr){
	if (!(lineNr < warehouse_row_relation.size()))
		{ return false; }
	warehouse_primary_key.erase(warehouse_row_relation[lineNr].w_id);
	warehouse_row_relation[lineNr] = warehouse_row_relation.back();
	warehouse_row_relation.pop_back();
	warehouse_primary_key[warehouse_row_relation[lineNr].w_id] = lineNr;
	return true;
}
bool district_delete(uint64_t lineNr){
	if (!(lineNr < district_row_relation.size()))
		{ return false; }
	district_primary_key[district_row_relation[lineNr].d_w_id].erase(district_row_relation[lineNr].d_id);
	district_row_relation[lineNr] = district_row_relation.back();
	district_row_relation.pop_back();
	district_primary_key[district_row_relation[lineNr].d_w_id][district_row_relation[lineNr].d_id] = lineNr;
	return true;
}
bool customer_delete(uint64_t lineNr){
	customer_wdl_index[customer_row_relation[lineNr].c_w_id][customer_row_relation[lineNr].c_d_id][customer_row_relation[lineNr].c_last].erase(customer_row_relation[lineNr].c_first);
	if (!(lineNr < customer_row_relation.size()))
		{ return false; }
	customer_primary_key[customer_row_relation[lineNr].c_w_id][customer_row_relation[lineNr].c_d_id].erase(customer_row_relation[lineNr].c_id);
	customer_row_relation[lineNr] = customer_row_relation.back();
	customer_row_relation.pop_back();
	customer_wdl_index[customer_row_relation[lineNr].c_w_id][customer_row_relation[lineNr].c_d_id][customer_row_relation[lineNr].c_last][customer_row_relation[lineNr].c_first] = lineNr;
	customer_primary_key[customer_row_relation[lineNr].c_w_id][customer_row_relation[lineNr].c_d_id][customer_row_relation[lineNr].c_id] = lineNr;
	return true;
}
bool history_delete(uint64_t lineNr){
	if (!(lineNr < history_row_relation.size()))
		{ return false; }
	history_row_relation[lineNr] = history_row_relation.back();
	history_row_relation.pop_back();
	return true;
}
bool neworder_delete(uint64_t lineNr){
	if (!(lineNr < neworder_row_relation.size()))
		{ return false; }
	neworder_primary_key[neworder_row_relation[lineNr].no_w_id][neworder_row_relation[lineNr].no_d_id].erase(neworder_row_relation[lineNr].no_o_id);
	neworder_row_relation[lineNr] = neworder_row_relation.back();
	neworder_row_relation.pop_back();
	neworder_primary_key[neworder_row_relation[lineNr].no_w_id][neworder_row_relation[lineNr].no_d_id][neworder_row_relation[lineNr].no_o_id] = lineNr;
	return true;
}
bool order_delete(uint64_t lineNr){
	order_wdc_index[order_row_relation[lineNr].o_w_id][order_row_relation[lineNr].o_d_id][order_row_relation[lineNr].o_c_id].erase(order_row_relation[lineNr].o_id);
	if (!(lineNr < order_row_relation.size()))
		{ return false; }
	order_primary_key[order_row_relation[lineNr].o_w_id][order_row_relation[lineNr].o_d_id].erase(order_row_relation[lineNr].o_id);
	order_row_relation[lineNr] = order_row_relation.back();
	order_row_relation.pop_back();
	order_wdc_index[order_row_relation[lineNr].o_w_id][order_row_relation[lineNr].o_d_id][order_row_relation[lineNr].o_c_id][order_row_relation[lineNr].o_id] = lineNr;
	order_primary_key[order_row_relation[lineNr].o_w_id][order_row_relation[lineNr].o_d_id][order_row_relation[lineNr].o_id] = lineNr;
	return true;
}
bool orderline_delete(uint64_t lineNr){
	if (!(lineNr < orderline_row_relation.size()))
		{ return false; }
	orderline_primary_key[orderline_row_relation[lineNr].ol_w_id][orderline_row_relation[lineNr].ol_d_id][orderline_row_relation[lineNr].ol_o_id].erase(orderline_row_relation[lineNr].ol_number);
	orderline_row_relation[lineNr] = orderline_row_relation.back();
	orderline_row_relation.pop_back();
	orderline_primary_key[orderline_row_relation[lineNr].ol_w_id][orderline_row_relation[lineNr].ol_d_id][orderline_row_relation[lineNr].ol_o_id][orderline_row_relation[lineNr].ol_number] = lineNr;
	return true;
}
bool item_delete(uint64_t lineNr){
	if (!(lineNr < item_row_relation.size()))
		{ return false; }
	item_primary_key.erase(item_row_relation[lineNr].i_id);
	item_row_relation[lineNr] = item_row_relation.back();
	item_row_relation.pop_back();
	item_primary_key[item_row_relation[lineNr].i_id] = lineNr;
	return true;
}
bool stock_delete(uint64_t lineNr){
	if (!(lineNr < stock_row_relation.size()))
		{ return false; }
	stock_primary_key[stock_row_relation[lineNr].s_w_id].erase(stock_row_relation[lineNr].s_i_id);
	stock_row_relation[lineNr] = stock_row_relation.back();
	stock_row_relation.pop_back();
	stock_primary_key[stock_row_relation[lineNr].s_w_id][stock_row_relation[lineNr].s_i_id] = lineNr;
	return true;
}

static void load_warehouse()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_warehouse.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			warehouse element;
			getline ( ss, token, '|' );
			element.w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_name = Varchar<10>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_city = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_state = Char<2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_zip = Char<9>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_tax = Numeric<4, 4>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.w_ytd = Numeric<12, 2>::castString ( token.c_str(), token.size() );
			warehouse_insert(element);
		}
		myfile.close();
	}
}
static void load_district()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_district.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			district element;
			getline ( ss, token, '|' );
			element.d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_name = Varchar<10>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_city = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_state = Char<2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_zip = Char<9>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_tax = Numeric<4, 4>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_ytd = Numeric<12, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.d_next_o_id = Integer::castString ( token.c_str(), token.size() );
			district_insert(element);
		}
		myfile.close();
	}
}
static void load_customer()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_customer.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			customer element;
			getline ( ss, token, '|' );
			element.c_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_first = Varchar<16>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_middle = Char<2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_last = Varchar<16>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_street_1 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_street_2 = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_city = Varchar<20>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_state = Char<2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_zip = Char<9>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_phone = Char<16>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_since = Timestamp::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_credit = Char<2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_credit_lim = Numeric<12, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_discount = Numeric<4, 4>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_balance = Numeric<12, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_ytd_paymenr = Numeric<12, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_payment_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_delivery_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.c_data = Varchar<500>::castString ( token.c_str(), token.size() );
			customer_insert(element);
		}
		myfile.close();
	}
}
static void load_history()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_history.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			history element;
			getline ( ss, token, '|' );
			element.h_c_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_c_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_c_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_date = Timestamp::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_amount = Numeric<6, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.h_data = Varchar<24>::castString ( token.c_str(), token.size() );
			history_insert(element);
		}
		myfile.close();
	}
}
static void load_neworder()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_neworder.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			neworder element;
			getline ( ss, token, '|' );
			element.no_o_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.no_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.no_w_id = Integer::castString ( token.c_str(), token.size() );
			neworder_insert(element);
		}
		myfile.close();
	}
}
static void load_order()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_order.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			order element;
			getline ( ss, token, '|' );
			element.o_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_c_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_entry_d = Timestamp::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_carrier_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_ol_cnt = Numeric<2, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.o_all_local = Numeric<1, 0>::castString ( token.c_str(), token.size() );
			order_insert(element);
		}
		myfile.close();
	}
}
static void load_orderline()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_orderline.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			orderline element;
			getline ( ss, token, '|' );
			element.ol_o_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_d_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_number = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_i_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_supply_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_delivery_d = Timestamp::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_quantity = Numeric<2, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_amount = Numeric<6, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.ol_dist_info = Char<24>::castString ( token.c_str(), token.size() );
			orderline_insert(element);
		}
		myfile.close();
	}
}
static void load_item()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_item.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			item element;
			getline ( ss, token, '|' );
			element.i_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.i_im_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.i_name = Varchar<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.i_price = Numeric<5, 2>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.i_data = Varchar<50>::castString ( token.c_str(), token.size() );
			item_insert(element);
		}
		myfile.close();
	}
}
static void load_stock()
{
	string line;
	ifstream myfile;
	myfile.open ( "../tpc-c/tbl/tpcc_stock.tbl" );
	if ( myfile.is_open() ) 
	{
		while ( getline ( myfile,line ) )
		{
			istringstream ss ( line );
			string token;
			stock element;
			getline ( ss, token, '|' );
			element.s_i_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_w_id = Integer::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_quantity = Numeric<4, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_01 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_02 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_03 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_04 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_05 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_06 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_07 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_08 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_09 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_dist_10 = Char<24>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_ytd = Numeric<8, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_order_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_remote_cnt = Numeric<4, 0>::castString ( token.c_str(), token.size() );
			getline ( ss, token, '|' );
			element.s_data = Varchar<50>::castString ( token.c_str(), token.size() );
			stock_insert(element);
		}
		myfile.close();
	}
}

void init_tbl() {
	load_warehouse();
	load_district();
	load_customer();
	load_history();
	load_neworder();
	load_order();
	load_orderline();
	load_item();
	load_stock();
}
