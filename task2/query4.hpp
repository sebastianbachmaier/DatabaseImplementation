void query4()
{
	std::unordered_map<tuple<Integer, Integer, Integer>, tuple<Varchar<16>, Varchar<16>, Numeric<1, 0>>> hashtable_op1;
	std::unordered_map<tuple<Integer, Integer, Integer>, tuple<Varchar<16>, Varchar<16>>> hashtable_op3;
	for (unsigned var_customer_line = 0; var_customer_line < customer_column_relation.size(); var_customer_line++)
	{
		Integer& c_id = customer_column_relation.c_id.at(var_customer_line);
		Integer& c_d_id = customer_column_relation.c_d_id.at(var_customer_line);
		Integer& c_w_id = customer_column_relation.c_w_id.at(var_customer_line);
		Varchar<16>& c_first = customer_column_relation.c_first.at(var_customer_line);
		Varchar<16>& c_last = customer_column_relation.c_last.at(var_customer_line);
		if (c_id == Integer(322) && c_w_id == Integer(1) && c_d_id == Integer(1) && true)
		{
			hashtable_op3.insert({std::make_tuple(c_id, c_d_id, c_w_id), std::make_tuple(c_first, c_last)});
		}
	}
	for (unsigned var_order_line = 0; var_order_line < order_column_relation.size(); var_order_line++)
	{
		Integer& o_id = order_column_relation.o_id.at(var_order_line);
		Integer& o_d_id = order_column_relation.o_d_id.at(var_order_line);
		Integer& o_w_id = order_column_relation.o_w_id.at(var_order_line);
		Integer& o_c_id = order_column_relation.o_c_id.at(var_order_line);
		Numeric<1, 0>& o_all_local = order_column_relation.o_all_local.at(var_order_line);
		auto key3 = std::make_tuple(o_d_id, o_w_id, o_c_id);
		if ( hashtable_op3.count (key3) > 0 )
		{
			auto& value = hashtable_op3.at(key3);
			Varchar<16>& c_first = std::get<0>(value);
			Varchar<16>& c_last = std::get<1>(value);
			/// Integer o_id;
			/// Integer o_d_id;
			/// Integer o_w_id;
			/// Numeric<1, 0> o_all_local;
			hashtable_op1.insert({std::make_tuple(o_id, o_d_id, o_w_id), std::make_tuple(c_first, c_last, o_all_local)});
		}
	}
	for (unsigned var_orderline_line = 0; var_orderline_line < orderline_column_relation.size(); var_orderline_line++)
	{
		Integer& ol_o_id = orderline_column_relation.ol_o_id.at(var_orderline_line);
		Integer& ol_d_id = orderline_column_relation.ol_d_id.at(var_orderline_line);
		Integer& ol_w_id = orderline_column_relation.ol_w_id.at(var_orderline_line);
		Numeric<6, 2>& ol_amount = orderline_column_relation.ol_amount.at(var_orderline_line);
		auto key1 = std::make_tuple(ol_o_id, ol_d_id, ol_w_id);
		if ( hashtable_op1.count (key1) > 0 )
		{
			auto& value = hashtable_op1.at(key1);
			Varchar<16>& c_first = std::get<0>(value);
			Varchar<16>& c_last = std::get<1>(value);
			Numeric<1, 0>& o_all_local = std::get<2>(value);
			/// Numeric<6, 2> ol_amount;
			tree->out << c_first << std::endl;
			tree->out << c_last << std::endl;
			tree->out << o_all_local << std::endl;
			tree->out << ol_amount << std::endl;
		}
	}
}
