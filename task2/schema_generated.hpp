#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <exception>
#include "Types.hpp"
#include <chrono>
using namespace std::chrono;
using namespace std;
string warehouse_name = "warehouse";
string district_name = "district";
string customer_name = "customer";
string history_name = "history";
string neworder_name = "neworder";
string order_name = "order";
string orderline_name = "orderline";
string item_name = "item";
string stock_name = "stock";


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
std::vector<warehouse> warehouse_row_relation;
std::vector<district> district_row_relation;
std::vector<customer> customer_row_relation;
std::vector<history> history_row_relation;
std::vector<neworder> neworder_row_relation;
std::vector<order> order_row_relation;
std::vector<orderline> orderline_row_relation;
std::vector<item> item_row_relation;
std::vector<stock> stock_row_relation;

struct warehouse_column{
std::vector<Integer> w_id;
std::vector<Varchar<10>> w_name;
std::vector<Varchar<20>> w_street_1;
std::vector<Varchar<20>> w_street_2;
std::vector<Varchar<20>> w_city;
std::vector<Char<2>> w_state;
std::vector<Char<9>> w_zip;
std::vector<Numeric<4, 4>> w_tax;
std::vector<Numeric<12, 2>> w_ytd;
};
warehouse_column warehouse_column_relation;

struct district_column{
std::vector<Integer> d_id;
std::vector<Integer> d_w_id;
std::vector<Varchar<10>> d_name;
std::vector<Varchar<20>> d_street_1;
std::vector<Varchar<20>> d_street_2;
std::vector<Varchar<20>> d_city;
std::vector<Char<2>> d_state;
std::vector<Char<9>> d_zip;
std::vector<Numeric<4, 4>> d_tax;
std::vector<Numeric<12, 2>> d_ytd;
std::vector<Integer> d_next_o_id;
};
district_column district_column_relation;

struct customer_column{
std::vector<Integer> c_id;
std::vector<Integer> c_d_id;
std::vector<Integer> c_w_id;
std::vector<Varchar<16>> c_first;
std::vector<Char<2>> c_middle;
std::vector<Varchar<16>> c_last;
std::vector<Varchar<20>> c_street_1;
std::vector<Varchar<20>> c_street_2;
std::vector<Varchar<20>> c_city;
std::vector<Char<2>> c_state;
std::vector<Char<9>> c_zip;
std::vector<Char<16>> c_phone;
std::vector<Timestamp> c_since;
std::vector<Char<2>> c_credit;
std::vector<Numeric<12, 2>> c_credit_lim;
std::vector<Numeric<4, 4>> c_discount;
std::vector<Numeric<12, 2>> c_balance;
std::vector<Numeric<12, 2>> c_ytd_paymenr;
std::vector<Numeric<4, 0>> c_payment_cnt;
std::vector<Numeric<4, 0>> c_delivery_cnt;
std::vector<Varchar<500>> c_data;
};
customer_column customer_column_relation;

struct history_column{
std::vector<Integer> h_c_id;
std::vector<Integer> h_c_d_id;
std::vector<Integer> h_c_w_id;
std::vector<Integer> h_d_id;
std::vector<Integer> h_w_id;
std::vector<Timestamp> h_date;
std::vector<Numeric<6, 2>> h_amount;
std::vector<Varchar<24>> h_data;
};
history_column history_column_relation;

struct neworder_column{
std::vector<Integer> no_o_id;
std::vector<Integer> no_d_id;
std::vector<Integer> no_w_id;
};
neworder_column neworder_column_relation;

struct order_column{
std::vector<Integer> o_id;
std::vector<Integer> o_d_id;
std::vector<Integer> o_w_id;
std::vector<Integer> o_c_id;
std::vector<Timestamp> o_entry_d;
std::vector<Integer> o_carrier_id;
std::vector<Numeric<2, 0>> o_ol_cnt;
std::vector<Numeric<1, 0>> o_all_local;
};
order_column order_column_relation;

struct orderline_column{
std::vector<Integer> ol_o_id;
std::vector<Integer> ol_d_id;
std::vector<Integer> ol_w_id;
std::vector<Integer> ol_number;
std::vector<Integer> ol_i_id;
std::vector<Integer> ol_supply_w_id;
std::vector<Timestamp> ol_delivery_d;
std::vector<Numeric<2, 0>> ol_quantity;
std::vector<Numeric<6, 2>> ol_amount;
std::vector<Char<24>> ol_dist_info;
};
orderline_column orderline_column_relation;

struct item_column{
std::vector<Integer> i_id;
std::vector<Integer> i_im_id;
std::vector<Varchar<24>> i_name;
std::vector<Numeric<5, 2>> i_price;
std::vector<Varchar<50>> i_data;
};
item_column item_column_relation;

struct stock_column{
std::vector<Integer> s_i_id;
std::vector<Integer> s_w_id;
std::vector<Numeric<4, 0>> s_quantity;
std::vector<Char<24>> s_dist_01;
std::vector<Char<24>> s_dist_02;
std::vector<Char<24>> s_dist_03;
std::vector<Char<24>> s_dist_04;
std::vector<Char<24>> s_dist_05;
std::vector<Char<24>> s_dist_06;
std::vector<Char<24>> s_dist_07;
std::vector<Char<24>> s_dist_08;
std::vector<Char<24>> s_dist_09;
std::vector<Char<24>> s_dist_10;
std::vector<Numeric<8, 0>> s_ytd;
std::vector<Numeric<4, 0>> s_order_cnt;
std::vector<Numeric<4, 0>> s_remote_cnt;
std::vector<Varchar<50>> s_data;
};
stock_column stock_column_relation;

using warehouse_primary_key_t = std::tuple<Integer>;
using district_primary_key_t = std::tuple<Integer, Integer>;
using customer_primary_key_t = std::tuple<Integer, Integer, Integer>;
using neworder_primary_key_t = std::tuple<Integer, Integer, Integer>;
using order_primary_key_t = std::tuple<Integer, Integer, Integer>;
using orderline_primary_key_t = std::tuple<Integer, Integer, Integer, Integer>;
using item_primary_key_t = std::tuple<Integer>;
using stock_primary_key_t = std::tuple<Integer, Integer>;
std::map<warehouse_primary_key_t, uint64_t> warehouse_primary_key;
std::map<district_primary_key_t, uint64_t> district_primary_key;
std::map<customer_primary_key_t, uint64_t> customer_primary_key;
std::map<neworder_primary_key_t, uint64_t> neworder_primary_key;
std::map<order_primary_key_t, uint64_t> order_primary_key;
std::map<orderline_primary_key_t, uint64_t> orderline_primary_key;
std::map<item_primary_key_t, uint64_t> item_primary_key;
std::map<stock_primary_key_t, uint64_t> stock_primary_key;

using customer_wdl_index_key_t = std::tuple<Integer, Integer, Varchar<16>, Varchar<16>>;
using order_wdc_index_key_t = std::tuple<Integer, Integer, Integer, Integer>;
std::map<customer_wdl_index_key_t, uint64_t> customer_wdl_index;
std::map<order_wdc_index_key_t, uint64_t> order_wdc_index;
bool warehouse_column_insert(warehouse element){
	if (warehouse_primary_key.count(warehouse_primary_key_t(element.w_id)) != 0)
		return false;
	warehouse_column_relation.w_id.push_back(element.w_id);
	warehouse_column_relation.w_name.push_back(element.w_name);
	warehouse_column_relation.w_street_1.push_back(element.w_street_1);
	warehouse_column_relation.w_street_2.push_back(element.w_street_2);
	warehouse_column_relation.w_city.push_back(element.w_city);
	warehouse_column_relation.w_state.push_back(element.w_state);
	warehouse_column_relation.w_zip.push_back(element.w_zip);
	warehouse_column_relation.w_tax.push_back(element.w_tax);
	warehouse_column_relation.w_ytd.push_back(element.w_ytd);
	warehouse_primary_key.insert(std::pair<warehouse_primary_key_t, uint64_t>(warehouse_primary_key_t(element.w_id),warehouse_column_relation.w_id.size()-1));
	return true;
}
bool district_column_insert(district element){
	if (district_primary_key.count(district_primary_key_t(element.d_w_id, element.d_id)) != 0)
		return false;
	district_column_relation.d_id.push_back(element.d_id);
	district_column_relation.d_w_id.push_back(element.d_w_id);
	district_column_relation.d_name.push_back(element.d_name);
	district_column_relation.d_street_1.push_back(element.d_street_1);
	district_column_relation.d_street_2.push_back(element.d_street_2);
	district_column_relation.d_city.push_back(element.d_city);
	district_column_relation.d_state.push_back(element.d_state);
	district_column_relation.d_zip.push_back(element.d_zip);
	district_column_relation.d_tax.push_back(element.d_tax);
	district_column_relation.d_ytd.push_back(element.d_ytd);
	district_column_relation.d_next_o_id.push_back(element.d_next_o_id);
	district_primary_key.insert(std::pair<district_primary_key_t, uint64_t>(district_primary_key_t(element.d_w_id, element.d_id),district_column_relation.d_w_id.size()-1));
	return true;
}
bool customer_column_insert(customer element){
	if (customer_primary_key.count(customer_primary_key_t(element.c_w_id, element.c_d_id, element.c_id)) != 0)
		return false;
	customer_column_relation.c_id.push_back(element.c_id);
	customer_column_relation.c_d_id.push_back(element.c_d_id);
	customer_column_relation.c_w_id.push_back(element.c_w_id);
	customer_column_relation.c_first.push_back(element.c_first);
	customer_column_relation.c_middle.push_back(element.c_middle);
	customer_column_relation.c_last.push_back(element.c_last);
	customer_column_relation.c_street_1.push_back(element.c_street_1);
	customer_column_relation.c_street_2.push_back(element.c_street_2);
	customer_column_relation.c_city.push_back(element.c_city);
	customer_column_relation.c_state.push_back(element.c_state);
	customer_column_relation.c_zip.push_back(element.c_zip);
	customer_column_relation.c_phone.push_back(element.c_phone);
	customer_column_relation.c_since.push_back(element.c_since);
	customer_column_relation.c_credit.push_back(element.c_credit);
	customer_column_relation.c_credit_lim.push_back(element.c_credit_lim);
	customer_column_relation.c_discount.push_back(element.c_discount);
	customer_column_relation.c_balance.push_back(element.c_balance);
	customer_column_relation.c_ytd_paymenr.push_back(element.c_ytd_paymenr);
	customer_column_relation.c_payment_cnt.push_back(element.c_payment_cnt);
	customer_column_relation.c_delivery_cnt.push_back(element.c_delivery_cnt);
	customer_column_relation.c_data.push_back(element.c_data);
	customer_primary_key.insert(std::pair<customer_primary_key_t, uint64_t>(customer_primary_key_t(element.c_w_id, element.c_d_id, element.c_id),customer_column_relation.c_w_id.size()-1));
	customer_wdl_index.insert(std::pair<customer_wdl_index_key_t, uint64_t>(customer_wdl_index_key_t(element.c_w_id, element.c_d_id, element.c_last, element.c_first), customer_column_relation.c_w_id.size()-1));
	return true;
}
bool history_column_insert(history element){
	history_column_relation.h_c_id.push_back(element.h_c_id);
	history_column_relation.h_c_d_id.push_back(element.h_c_d_id);
	history_column_relation.h_c_w_id.push_back(element.h_c_w_id);
	history_column_relation.h_d_id.push_back(element.h_d_id);
	history_column_relation.h_w_id.push_back(element.h_w_id);
	history_column_relation.h_date.push_back(element.h_date);
	history_column_relation.h_amount.push_back(element.h_amount);
	history_column_relation.h_data.push_back(element.h_data);
	return true;
}
bool neworder_column_insert(neworder element){
	if (neworder_primary_key.count(neworder_primary_key_t(element.no_w_id, element.no_d_id, element.no_o_id)) != 0)
		return false;
	neworder_column_relation.no_o_id.push_back(element.no_o_id);
	neworder_column_relation.no_d_id.push_back(element.no_d_id);
	neworder_column_relation.no_w_id.push_back(element.no_w_id);
	neworder_primary_key.insert(std::pair<neworder_primary_key_t, uint64_t>(neworder_primary_key_t(element.no_w_id, element.no_d_id, element.no_o_id),neworder_column_relation.no_w_id.size()-1));
	return true;
}
bool order_column_insert(order element){
	if (order_primary_key.count(order_primary_key_t(element.o_w_id, element.o_d_id, element.o_id)) != 0)
		return false;
	order_column_relation.o_id.push_back(element.o_id);
	order_column_relation.o_d_id.push_back(element.o_d_id);
	order_column_relation.o_w_id.push_back(element.o_w_id);
	order_column_relation.o_c_id.push_back(element.o_c_id);
	order_column_relation.o_entry_d.push_back(element.o_entry_d);
	order_column_relation.o_carrier_id.push_back(element.o_carrier_id);
	order_column_relation.o_ol_cnt.push_back(element.o_ol_cnt);
	order_column_relation.o_all_local.push_back(element.o_all_local);
	order_primary_key.insert(std::pair<order_primary_key_t, uint64_t>(order_primary_key_t(element.o_w_id, element.o_d_id, element.o_id),order_column_relation.o_w_id.size()-1));
	order_wdc_index.insert(std::pair<order_wdc_index_key_t, uint64_t>(order_wdc_index_key_t(element.o_w_id, element.o_d_id, element.o_c_id, element.o_id), order_column_relation.o_w_id.size()-1));
	return true;
}
bool orderline_column_insert(orderline element){
	if (orderline_primary_key.count(orderline_primary_key_t(element.ol_w_id, element.ol_d_id, element.ol_o_id, element.ol_number)) != 0)
		return false;
	orderline_column_relation.ol_o_id.push_back(element.ol_o_id);
	orderline_column_relation.ol_d_id.push_back(element.ol_d_id);
	orderline_column_relation.ol_w_id.push_back(element.ol_w_id);
	orderline_column_relation.ol_number.push_back(element.ol_number);
	orderline_column_relation.ol_i_id.push_back(element.ol_i_id);
	orderline_column_relation.ol_supply_w_id.push_back(element.ol_supply_w_id);
	orderline_column_relation.ol_delivery_d.push_back(element.ol_delivery_d);
	orderline_column_relation.ol_quantity.push_back(element.ol_quantity);
	orderline_column_relation.ol_amount.push_back(element.ol_amount);
	orderline_column_relation.ol_dist_info.push_back(element.ol_dist_info);
	orderline_primary_key.insert(std::pair<orderline_primary_key_t, uint64_t>(orderline_primary_key_t(element.ol_w_id, element.ol_d_id, element.ol_o_id, element.ol_number),orderline_column_relation.ol_w_id.size()-1));
	return true;
}
bool item_column_insert(item element){
	if (item_primary_key.count(item_primary_key_t(element.i_id)) != 0)
		return false;
	item_column_relation.i_id.push_back(element.i_id);
	item_column_relation.i_im_id.push_back(element.i_im_id);
	item_column_relation.i_name.push_back(element.i_name);
	item_column_relation.i_price.push_back(element.i_price);
	item_column_relation.i_data.push_back(element.i_data);
	item_primary_key.insert(std::pair<item_primary_key_t, uint64_t>(item_primary_key_t(element.i_id),item_column_relation.i_id.size()-1));
	return true;
}
bool stock_column_insert(stock element){
	if (stock_primary_key.count(stock_primary_key_t(element.s_w_id, element.s_i_id)) != 0)
		return false;
	stock_column_relation.s_i_id.push_back(element.s_i_id);
	stock_column_relation.s_w_id.push_back(element.s_w_id);
	stock_column_relation.s_quantity.push_back(element.s_quantity);
	stock_column_relation.s_dist_01.push_back(element.s_dist_01);
	stock_column_relation.s_dist_02.push_back(element.s_dist_02);
	stock_column_relation.s_dist_03.push_back(element.s_dist_03);
	stock_column_relation.s_dist_04.push_back(element.s_dist_04);
	stock_column_relation.s_dist_05.push_back(element.s_dist_05);
	stock_column_relation.s_dist_06.push_back(element.s_dist_06);
	stock_column_relation.s_dist_07.push_back(element.s_dist_07);
	stock_column_relation.s_dist_08.push_back(element.s_dist_08);
	stock_column_relation.s_dist_09.push_back(element.s_dist_09);
	stock_column_relation.s_dist_10.push_back(element.s_dist_10);
	stock_column_relation.s_ytd.push_back(element.s_ytd);
	stock_column_relation.s_order_cnt.push_back(element.s_order_cnt);
	stock_column_relation.s_remote_cnt.push_back(element.s_remote_cnt);
	stock_column_relation.s_data.push_back(element.s_data);
	stock_primary_key.insert(std::pair<stock_primary_key_t, uint64_t>(stock_primary_key_t(element.s_w_id, element.s_i_id),stock_column_relation.s_w_id.size()-1));
	return true;
}
bool warehouse_column_delete(uint64_t lineNr){
	if (lineNr >= warehouse_column_relation.w_id.size())
		return false;
	warehouse_primary_key.erase(warehouse_primary_key_t(warehouse_column_relation.w_id[lineNr]));
	if (warehouse_column_relation.w_id.size() > 1 && lineNr != warehouse_column_relation.w_id.size()-1){
		warehouse_column_relation.w_id[lineNr] = warehouse_column_relation.w_id.back();
		warehouse_column_relation.w_name[lineNr] = warehouse_column_relation.w_name.back();
		warehouse_column_relation.w_street_1[lineNr] = warehouse_column_relation.w_street_1.back();
		warehouse_column_relation.w_street_2[lineNr] = warehouse_column_relation.w_street_2.back();
		warehouse_column_relation.w_city[lineNr] = warehouse_column_relation.w_city.back();
		warehouse_column_relation.w_state[lineNr] = warehouse_column_relation.w_state.back();
		warehouse_column_relation.w_zip[lineNr] = warehouse_column_relation.w_zip.back();
		warehouse_column_relation.w_tax[lineNr] = warehouse_column_relation.w_tax.back();
		warehouse_column_relation.w_ytd[lineNr] = warehouse_column_relation.w_ytd.back();
		warehouse_primary_key[warehouse_primary_key_t(warehouse_column_relation.w_id.at(lineNr))] = lineNr;
	}
	warehouse_column_relation.w_id.pop_back();
	warehouse_column_relation.w_name.pop_back();
	warehouse_column_relation.w_street_1.pop_back();
	warehouse_column_relation.w_street_2.pop_back();
	warehouse_column_relation.w_city.pop_back();
	warehouse_column_relation.w_state.pop_back();
	warehouse_column_relation.w_zip.pop_back();
	warehouse_column_relation.w_tax.pop_back();
	warehouse_column_relation.w_ytd.pop_back();
	return true;
}
bool district_column_delete(uint64_t lineNr){
	if (lineNr >= district_column_relation.d_id.size())
		return false;
	district_primary_key.erase(district_primary_key_t(district_column_relation.d_w_id[lineNr], district_column_relation.d_id[lineNr]));
	if (district_column_relation.d_id.size() > 1 && lineNr != district_column_relation.d_id.size()-1){
		district_column_relation.d_id[lineNr] = district_column_relation.d_id.back();
		district_column_relation.d_w_id[lineNr] = district_column_relation.d_w_id.back();
		district_column_relation.d_name[lineNr] = district_column_relation.d_name.back();
		district_column_relation.d_street_1[lineNr] = district_column_relation.d_street_1.back();
		district_column_relation.d_street_2[lineNr] = district_column_relation.d_street_2.back();
		district_column_relation.d_city[lineNr] = district_column_relation.d_city.back();
		district_column_relation.d_state[lineNr] = district_column_relation.d_state.back();
		district_column_relation.d_zip[lineNr] = district_column_relation.d_zip.back();
		district_column_relation.d_tax[lineNr] = district_column_relation.d_tax.back();
		district_column_relation.d_ytd[lineNr] = district_column_relation.d_ytd.back();
		district_column_relation.d_next_o_id[lineNr] = district_column_relation.d_next_o_id.back();
		district_primary_key[district_primary_key_t(district_column_relation.d_w_id.at(lineNr), district_column_relation.d_id.at(lineNr))] = lineNr;
	}
	district_column_relation.d_id.pop_back();
	district_column_relation.d_w_id.pop_back();
	district_column_relation.d_name.pop_back();
	district_column_relation.d_street_1.pop_back();
	district_column_relation.d_street_2.pop_back();
	district_column_relation.d_city.pop_back();
	district_column_relation.d_state.pop_back();
	district_column_relation.d_zip.pop_back();
	district_column_relation.d_tax.pop_back();
	district_column_relation.d_ytd.pop_back();
	district_column_relation.d_next_o_id.pop_back();
	return true;
}
bool customer_column_delete(uint64_t lineNr){
	if (lineNr >= customer_column_relation.c_id.size())
		return false;
	customer_wdl_index.erase(customer_wdl_index_key_t(customer_column_relation.c_w_id[lineNr], customer_column_relation.c_d_id[lineNr], customer_column_relation.c_last[lineNr], customer_column_relation.c_first[lineNr]));
	customer_primary_key.erase(customer_primary_key_t(customer_column_relation.c_w_id[lineNr], customer_column_relation.c_d_id[lineNr], customer_column_relation.c_id[lineNr]));
	if (customer_column_relation.c_id.size() > 1 && lineNr != customer_column_relation.c_id.size()-1){
		customer_column_relation.c_id[lineNr] = customer_column_relation.c_id.back();
		customer_column_relation.c_d_id[lineNr] = customer_column_relation.c_d_id.back();
		customer_column_relation.c_w_id[lineNr] = customer_column_relation.c_w_id.back();
		customer_column_relation.c_first[lineNr] = customer_column_relation.c_first.back();
		customer_column_relation.c_middle[lineNr] = customer_column_relation.c_middle.back();
		customer_column_relation.c_last[lineNr] = customer_column_relation.c_last.back();
		customer_column_relation.c_street_1[lineNr] = customer_column_relation.c_street_1.back();
		customer_column_relation.c_street_2[lineNr] = customer_column_relation.c_street_2.back();
		customer_column_relation.c_city[lineNr] = customer_column_relation.c_city.back();
		customer_column_relation.c_state[lineNr] = customer_column_relation.c_state.back();
		customer_column_relation.c_zip[lineNr] = customer_column_relation.c_zip.back();
		customer_column_relation.c_phone[lineNr] = customer_column_relation.c_phone.back();
		customer_column_relation.c_since[lineNr] = customer_column_relation.c_since.back();
		customer_column_relation.c_credit[lineNr] = customer_column_relation.c_credit.back();
		customer_column_relation.c_credit_lim[lineNr] = customer_column_relation.c_credit_lim.back();
		customer_column_relation.c_discount[lineNr] = customer_column_relation.c_discount.back();
		customer_column_relation.c_balance[lineNr] = customer_column_relation.c_balance.back();
		customer_column_relation.c_ytd_paymenr[lineNr] = customer_column_relation.c_ytd_paymenr.back();
		customer_column_relation.c_payment_cnt[lineNr] = customer_column_relation.c_payment_cnt.back();
		customer_column_relation.c_delivery_cnt[lineNr] = customer_column_relation.c_delivery_cnt.back();
		customer_column_relation.c_data[lineNr] = customer_column_relation.c_data.back();
		customer_primary_key[customer_primary_key_t(customer_column_relation.c_w_id.at(lineNr), customer_column_relation.c_d_id.at(lineNr), customer_column_relation.c_id.at(lineNr))] = lineNr;
		customer_wdl_index[customer_wdl_index_key_t(customer_column_relation.c_w_id.at(lineNr), customer_column_relation.c_d_id.at(lineNr), customer_column_relation.c_last.at(lineNr), customer_column_relation.c_first.at(lineNr))] = lineNr;
	}
	customer_column_relation.c_id.pop_back();
	customer_column_relation.c_d_id.pop_back();
	customer_column_relation.c_w_id.pop_back();
	customer_column_relation.c_first.pop_back();
	customer_column_relation.c_middle.pop_back();
	customer_column_relation.c_last.pop_back();
	customer_column_relation.c_street_1.pop_back();
	customer_column_relation.c_street_2.pop_back();
	customer_column_relation.c_city.pop_back();
	customer_column_relation.c_state.pop_back();
	customer_column_relation.c_zip.pop_back();
	customer_column_relation.c_phone.pop_back();
	customer_column_relation.c_since.pop_back();
	customer_column_relation.c_credit.pop_back();
	customer_column_relation.c_credit_lim.pop_back();
	customer_column_relation.c_discount.pop_back();
	customer_column_relation.c_balance.pop_back();
	customer_column_relation.c_ytd_paymenr.pop_back();
	customer_column_relation.c_payment_cnt.pop_back();
	customer_column_relation.c_delivery_cnt.pop_back();
	customer_column_relation.c_data.pop_back();
	return true;
}
bool history_column_delete(uint64_t lineNr){
	if (lineNr >= history_column_relation.h_c_id.size())
		return false;
	if (history_column_relation.h_c_id.size() > 1 && lineNr != history_column_relation.h_c_id.size()-1){
		history_column_relation.h_c_id[lineNr] = history_column_relation.h_c_id.back();
		history_column_relation.h_c_d_id[lineNr] = history_column_relation.h_c_d_id.back();
		history_column_relation.h_c_w_id[lineNr] = history_column_relation.h_c_w_id.back();
		history_column_relation.h_d_id[lineNr] = history_column_relation.h_d_id.back();
		history_column_relation.h_w_id[lineNr] = history_column_relation.h_w_id.back();
		history_column_relation.h_date[lineNr] = history_column_relation.h_date.back();
		history_column_relation.h_amount[lineNr] = history_column_relation.h_amount.back();
		history_column_relation.h_data[lineNr] = history_column_relation.h_data.back();
	}
	history_column_relation.h_c_id.pop_back();
	history_column_relation.h_c_d_id.pop_back();
	history_column_relation.h_c_w_id.pop_back();
	history_column_relation.h_d_id.pop_back();
	history_column_relation.h_w_id.pop_back();
	history_column_relation.h_date.pop_back();
	history_column_relation.h_amount.pop_back();
	history_column_relation.h_data.pop_back();
	return true;
}
bool neworder_column_delete(uint64_t lineNr){
	if (lineNr >= neworder_column_relation.no_o_id.size())
		return false;
	neworder_primary_key.erase(neworder_primary_key_t(neworder_column_relation.no_w_id[lineNr], neworder_column_relation.no_d_id[lineNr], neworder_column_relation.no_o_id[lineNr]));
	if (neworder_column_relation.no_o_id.size() > 1 && lineNr != neworder_column_relation.no_o_id.size()-1){
		neworder_column_relation.no_o_id[lineNr] = neworder_column_relation.no_o_id.back();
		neworder_column_relation.no_d_id[lineNr] = neworder_column_relation.no_d_id.back();
		neworder_column_relation.no_w_id[lineNr] = neworder_column_relation.no_w_id.back();
		neworder_primary_key[neworder_primary_key_t(neworder_column_relation.no_w_id.at(lineNr), neworder_column_relation.no_d_id.at(lineNr), neworder_column_relation.no_o_id.at(lineNr))] = lineNr;
	}
	neworder_column_relation.no_o_id.pop_back();
	neworder_column_relation.no_d_id.pop_back();
	neworder_column_relation.no_w_id.pop_back();
	return true;
}
bool order_column_delete(uint64_t lineNr){
	if (lineNr >= order_column_relation.o_id.size())
		return false;
	order_wdc_index.erase(order_wdc_index_key_t(order_column_relation.o_w_id[lineNr], order_column_relation.o_d_id[lineNr], order_column_relation.o_c_id[lineNr], order_column_relation.o_id[lineNr]));
	order_primary_key.erase(order_primary_key_t(order_column_relation.o_w_id[lineNr], order_column_relation.o_d_id[lineNr], order_column_relation.o_id[lineNr]));
	if (order_column_relation.o_id.size() > 1 && lineNr != order_column_relation.o_id.size()-1){
		order_column_relation.o_id[lineNr] = order_column_relation.o_id.back();
		order_column_relation.o_d_id[lineNr] = order_column_relation.o_d_id.back();
		order_column_relation.o_w_id[lineNr] = order_column_relation.o_w_id.back();
		order_column_relation.o_c_id[lineNr] = order_column_relation.o_c_id.back();
		order_column_relation.o_entry_d[lineNr] = order_column_relation.o_entry_d.back();
		order_column_relation.o_carrier_id[lineNr] = order_column_relation.o_carrier_id.back();
		order_column_relation.o_ol_cnt[lineNr] = order_column_relation.o_ol_cnt.back();
		order_column_relation.o_all_local[lineNr] = order_column_relation.o_all_local.back();
		order_primary_key[order_primary_key_t(order_column_relation.o_w_id.at(lineNr), order_column_relation.o_d_id.at(lineNr), order_column_relation.o_id.at(lineNr))] = lineNr;
		order_wdc_index[order_wdc_index_key_t(order_column_relation.o_w_id.at(lineNr), order_column_relation.o_d_id.at(lineNr), order_column_relation.o_c_id.at(lineNr), order_column_relation.o_id.at(lineNr))] = lineNr;
	}
	order_column_relation.o_id.pop_back();
	order_column_relation.o_d_id.pop_back();
	order_column_relation.o_w_id.pop_back();
	order_column_relation.o_c_id.pop_back();
	order_column_relation.o_entry_d.pop_back();
	order_column_relation.o_carrier_id.pop_back();
	order_column_relation.o_ol_cnt.pop_back();
	order_column_relation.o_all_local.pop_back();
	return true;
}
bool orderline_column_delete(uint64_t lineNr){
	if (lineNr >= orderline_column_relation.ol_o_id.size())
		return false;
	orderline_primary_key.erase(orderline_primary_key_t(orderline_column_relation.ol_w_id[lineNr], orderline_column_relation.ol_d_id[lineNr], orderline_column_relation.ol_o_id[lineNr], orderline_column_relation.ol_number[lineNr]));
	if (orderline_column_relation.ol_o_id.size() > 1 && lineNr != orderline_column_relation.ol_o_id.size()-1){
		orderline_column_relation.ol_o_id[lineNr] = orderline_column_relation.ol_o_id.back();
		orderline_column_relation.ol_d_id[lineNr] = orderline_column_relation.ol_d_id.back();
		orderline_column_relation.ol_w_id[lineNr] = orderline_column_relation.ol_w_id.back();
		orderline_column_relation.ol_number[lineNr] = orderline_column_relation.ol_number.back();
		orderline_column_relation.ol_i_id[lineNr] = orderline_column_relation.ol_i_id.back();
		orderline_column_relation.ol_supply_w_id[lineNr] = orderline_column_relation.ol_supply_w_id.back();
		orderline_column_relation.ol_delivery_d[lineNr] = orderline_column_relation.ol_delivery_d.back();
		orderline_column_relation.ol_quantity[lineNr] = orderline_column_relation.ol_quantity.back();
		orderline_column_relation.ol_amount[lineNr] = orderline_column_relation.ol_amount.back();
		orderline_column_relation.ol_dist_info[lineNr] = orderline_column_relation.ol_dist_info.back();
		orderline_primary_key[orderline_primary_key_t(orderline_column_relation.ol_w_id.at(lineNr), orderline_column_relation.ol_d_id.at(lineNr), orderline_column_relation.ol_o_id.at(lineNr), orderline_column_relation.ol_number.at(lineNr))] = lineNr;
	}
	orderline_column_relation.ol_o_id.pop_back();
	orderline_column_relation.ol_d_id.pop_back();
	orderline_column_relation.ol_w_id.pop_back();
	orderline_column_relation.ol_number.pop_back();
	orderline_column_relation.ol_i_id.pop_back();
	orderline_column_relation.ol_supply_w_id.pop_back();
	orderline_column_relation.ol_delivery_d.pop_back();
	orderline_column_relation.ol_quantity.pop_back();
	orderline_column_relation.ol_amount.pop_back();
	orderline_column_relation.ol_dist_info.pop_back();
	return true;
}
bool item_column_delete(uint64_t lineNr){
	if (lineNr >= item_column_relation.i_id.size())
		return false;
	item_primary_key.erase(item_primary_key_t(item_column_relation.i_id[lineNr]));
	if (item_column_relation.i_id.size() > 1 && lineNr != item_column_relation.i_id.size()-1){
		item_column_relation.i_id[lineNr] = item_column_relation.i_id.back();
		item_column_relation.i_im_id[lineNr] = item_column_relation.i_im_id.back();
		item_column_relation.i_name[lineNr] = item_column_relation.i_name.back();
		item_column_relation.i_price[lineNr] = item_column_relation.i_price.back();
		item_column_relation.i_data[lineNr] = item_column_relation.i_data.back();
		item_primary_key[item_primary_key_t(item_column_relation.i_id.at(lineNr))] = lineNr;
	}
	item_column_relation.i_id.pop_back();
	item_column_relation.i_im_id.pop_back();
	item_column_relation.i_name.pop_back();
	item_column_relation.i_price.pop_back();
	item_column_relation.i_data.pop_back();
	return true;
}
bool stock_column_delete(uint64_t lineNr){
	if (lineNr >= stock_column_relation.s_i_id.size())
		return false;
	stock_primary_key.erase(stock_primary_key_t(stock_column_relation.s_w_id[lineNr], stock_column_relation.s_i_id[lineNr]));
	if (stock_column_relation.s_i_id.size() > 1 && lineNr != stock_column_relation.s_i_id.size()-1){
		stock_column_relation.s_i_id[lineNr] = stock_column_relation.s_i_id.back();
		stock_column_relation.s_w_id[lineNr] = stock_column_relation.s_w_id.back();
		stock_column_relation.s_quantity[lineNr] = stock_column_relation.s_quantity.back();
		stock_column_relation.s_dist_01[lineNr] = stock_column_relation.s_dist_01.back();
		stock_column_relation.s_dist_02[lineNr] = stock_column_relation.s_dist_02.back();
		stock_column_relation.s_dist_03[lineNr] = stock_column_relation.s_dist_03.back();
		stock_column_relation.s_dist_04[lineNr] = stock_column_relation.s_dist_04.back();
		stock_column_relation.s_dist_05[lineNr] = stock_column_relation.s_dist_05.back();
		stock_column_relation.s_dist_06[lineNr] = stock_column_relation.s_dist_06.back();
		stock_column_relation.s_dist_07[lineNr] = stock_column_relation.s_dist_07.back();
		stock_column_relation.s_dist_08[lineNr] = stock_column_relation.s_dist_08.back();
		stock_column_relation.s_dist_09[lineNr] = stock_column_relation.s_dist_09.back();
		stock_column_relation.s_dist_10[lineNr] = stock_column_relation.s_dist_10.back();
		stock_column_relation.s_ytd[lineNr] = stock_column_relation.s_ytd.back();
		stock_column_relation.s_order_cnt[lineNr] = stock_column_relation.s_order_cnt.back();
		stock_column_relation.s_remote_cnt[lineNr] = stock_column_relation.s_remote_cnt.back();
		stock_column_relation.s_data[lineNr] = stock_column_relation.s_data.back();
		stock_primary_key[stock_primary_key_t(stock_column_relation.s_w_id.at(lineNr), stock_column_relation.s_i_id.at(lineNr))] = lineNr;
	}
	stock_column_relation.s_i_id.pop_back();
	stock_column_relation.s_w_id.pop_back();
	stock_column_relation.s_quantity.pop_back();
	stock_column_relation.s_dist_01.pop_back();
	stock_column_relation.s_dist_02.pop_back();
	stock_column_relation.s_dist_03.pop_back();
	stock_column_relation.s_dist_04.pop_back();
	stock_column_relation.s_dist_05.pop_back();
	stock_column_relation.s_dist_06.pop_back();
	stock_column_relation.s_dist_07.pop_back();
	stock_column_relation.s_dist_08.pop_back();
	stock_column_relation.s_dist_09.pop_back();
	stock_column_relation.s_dist_10.pop_back();
	stock_column_relation.s_ytd.pop_back();
	stock_column_relation.s_order_cnt.pop_back();
	stock_column_relation.s_remote_cnt.pop_back();
	stock_column_relation.s_data.pop_back();
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
			warehouse_column_insert(element);
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
			district_column_insert(element);
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
			customer_column_insert(element);
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
			history_column_insert(element);
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
			neworder_column_insert(element);
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
			order_column_insert(element);
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
			orderline_column_insert(element);
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
			item_column_insert(element);
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
			stock_column_insert(element);
		}
		myfile.close();
	}
}

void init_tbl() {
	 auto start=high_resolution_clock::now();
	start=high_resolution_clock::now();
	load_warehouse();
	cout << "Load: warehouse: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_district();
	cout << "Load: district: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_customer();
	cout << "Load: customer: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_history();
	cout << "Load: history: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_neworder();
	cout << "Load: neworder: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_order();
	cout << "Load: order: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_orderline();
	cout << "Load: orderline: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_item();
	cout << "Load: item: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
	start=high_resolution_clock::now();
	load_stock();
	cout << "Load: stock: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
}
