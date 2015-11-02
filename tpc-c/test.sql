create table warehouse (
	w_id integer not null,
	w_zip char(9) not null,
	w_state char(2) not null,
	bla varchar(3) not null,
	w_tax numeric(4,4) not null,
	w_ytd numeric(12,2) not null,
	h_date timestamp not null,
	primary key (w_id,w_zip)
);