#!/bin/bash
Var="struct warehouse (
	w_id integer 
	w_name varchar(10) 
	w_street_1 varchar(20) 
	w_street_2 varchar(20) 
	w_city varchar(20) 
	w_state char(2) 
	w_zip char(9) 
	w_tax numeric(4,4) 
	w_ytd numeric(12,2) 
	primary key (w_id)
);

struct district (
	d_id integer 
	d_w_id integer 
	d_name varchar(10) 
	d_street_1 varchar(20) 
	d_street_2 varchar(20) 
	d_city varchar(20) 
	d_state char(2) 
	d_zip char(9) 
	d_tax numeric(4,4) 
	d_ytd numeric(12,2) 
	d_next_o_id integer 
	primary key (d_w_id,d_id)
);

struct customer (
	c_id integer 
	c_d_id integer 
	c_w_id integer 
	c_first varchar(16) 
	c_middle char(2) 
	c_last varchar(16) 
	c_street_1 varchar(20) 
	c_street_2 varchar(20) 
	c_city varchar(20) 
	c_state char(2) 
	c_zip char(9) 
	c_phone char(16) 
	c_since timestamp 
	c_credit char(2) 
	c_credit_lim numeric(12,2) 
	c_discount numeric(4,4) 
	c_balance numeric(12,2) 
	c_ytd_paymenr numeric(12,2) 
	c_payment_cnt numeric(4,0) 
	c_delivery_cnt numeric(4,0) 
	c_data varchar(500) 
	primary key (c_w_id,c_d_id,c_id)
);

create index customer_wdl on customer(c_w_id,c_d_id,c_last,c_first);

struct history (
	h_c_id integer 
	h_c_d_id integer 
	h_c_w_id integer 
	h_d_id integer 
	h_w_id integer 
	h_date timestamp 
	h_amount numeric(6,2) 
	h_data varchar(24) not null
);

struct neworder (
	no_o_id integer 
	no_d_id integer 
	no_w_id integer 
	primary key (no_w_id,no_d_id,no_o_id)
);

struct order (
	o_id integer 
	o_d_id integer 
	o_w_id integer 
	o_c_id integer 
	o_entry_d timestamp 
	o_carrier_id integer 
	o_ol_cnt numeric(2,0) 
	o_all_local numeric(1,0) 
	primary key (o_w_id,o_d_id,o_id)
);

create index order_wdc on order(o_w_id,o_d_id,o_c_id,o_id);

struct orderline (
	ol_o_id integer 
	ol_d_id integer 
	ol_w_id integer 
	ol_number integer 
	ol_i_id integer 
	ol_supply_w_id integer 
	ol_delivery_d timestamp 
	ol_quantity numeric(2,0) 
	ol_amount numeric(6,2) 
	ol_dist_info char(24) 
	primary key (ol_w_id,ol_d_id,ol_o_id,ol_number)
);

struct item (
	i_id integer 
	i_im_id integer 
	i_name varchar(24) 
	i_price numeric(5,2) 
	i_data varchar(50) 
	primary key (i_id)
);

struct stock (
	s_i_id integer 
	s_w_id integer 
	s_quantity numeric(4,0) 
	s_dist_01 char(24) 
	s_dist_02 char(24) 
	s_dist_03 char(24) 
	s_dist_04 char(24) 
	s_dist_05 char(24) 
	s_dist_06 char(24) 
	s_dist_07 char(24) 
	s_dist_08 char(24) 
	s_dist_09 char(24) 
	s_dist_10 char(24) 
	s_ytd numeric(8,0) 
	s_order_cnt numeric(4,0) 
	s_remote_cnt numeric(4,0) 
	s_data varchar(50) 
	primary key (s_w_id,s_i_id)
);"

echo $Var | awk  'arr[NR]=$0; END{for(i=0; i<NR;i++) {print arr[i]}}'
