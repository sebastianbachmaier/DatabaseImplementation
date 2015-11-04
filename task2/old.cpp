#if 0
double query()
{
  /*
    select sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
from customer, "order", orderline
and c_last like 'B%'

where o_w_id = c_w_id
and o_d_id = c_d_id
and o_c_id = c_id

and o_w_id = ol_w_id
and o_d_id = ol_d_id
and o_id = ol_o_id
*/
    
    double result = 0;
    
    // c_last like %B
    uint32_t customer_like_size = 0;

    uint32_t* customer_like = new uint32_t[customer_column_relation.c_id.size()];


    for (uint32_t customer = 0; customer < customer_column_relation.c_last.size(); customer++)
    {
        Varchar <16>& c_last = customer_column_relation.c_last[customer];
        if (*c_last.value == 'B')
        {
           
            customer_like[customer_like_size] = customer;
            customer_like_size++;
        }
    }
    
     
    /*
     *  o_c_id = c_id and c_d_id = o_d_id and o_w_id = c_w_id
     *  PrimaryKey so it's a 1 to N at least
     */
   
    {
        
        ChainingHT<tuple<Integer, Integer, Integer>, uint32_t> *HC = new ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>(customer_like_size);

        ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry* e = static_cast<ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry*>(mmap(NULL, customer_like_size*sizeof(ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
        madvise(e, customer_like_size, MADV_HUGEPAGE);

        parallel_for(blocked_range<size_t>(0, customer_like_size), [&](const blocked_range<size_t>& range) {
            for (size_t i=range.begin(); i!=range.end(); ++i){
                e[i].key = tuple<Integer, Integer, Integer>(
                    customer_column_relation.c_id[customer_like[i]], 
                    customer_column_relation.c_d_id[customer_like[i]], 
                    customer_column_relation.c_w_id[customer_like[i]]
                 );
                    
                e[i].value = customer_like[i];

                HC->insert(e+i);
            }

        });
        //cout << "Build Finished\n";
        // probe o_c_id and build
        /*
        *  PrimaryKey so it's a 1 to N at least
        * and o_w_id = ol_w_id
        * and o_d_id = ol_d_id
        * and o_id = ol_o_id
        */  
        
        // there are as many customers as orders (see primary key)
        ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>> *HC2 = new ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>(customer_like_size);

        ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry* e2 = static_cast<ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry*>(mmap(NULL, customer_like_size*sizeof(ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
        madvise(e, customer_like_size, MADV_HUGEPAGE);
        
        uint32_t j = 0;

        for (uint32_t i = 0;i < order_column_relation.o_c_id.size();i++) {
            auto key = tuple<Integer, Integer, Integer>(
                    order_column_relation.o_c_id[i], 
                    order_column_relation.o_d_id[i], 
                    order_column_relation.o_w_id[i]
                 );
            const auto E = HC->lookup(key);
            if (E != NULL) {
                get<0>(key) = order_column_relation.o_id[i];
                e2[j].key = key;
                e2[j].value = make_tuple(E->value,i); /// tuple from customer and order
                HC2->insert(e2+j); /// pipline result forward to next table
                j++;
            }
        }
        
        
        
        //cout << "Probe and Build Finished\n";
        munmap(e, customer_like_size*sizeof(typename ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry));
        delete HC;

        // probe o_c_id and compute result       
        for (uint32_t i = 0;i < orderline_column_relation.ol_o_id.size();i++) {
            const auto E = HC2->lookup(
                tuple<Integer, Integer, Integer>(
                    orderline_column_relation.ol_o_id[i], 
                    orderline_column_relation.ol_d_id[i], 
                    orderline_column_relation.ol_w_id[i]
                 )
              );
            if (E != NULL) {
                result += (((double)orderline_column_relation.ol_quantity[i] *
            ((double)orderline_column_relation.ol_amount[i]))-
            (((double)customer_column_relation.c_balance[get<0>(E->value)]) *
            ((double)order_column_relation.o_ol_cnt[get<1>(E->value)])));
                //customer_order_orderline.insert(std::tuple_cat(E->value,std::make_tuple(i)));
            }
        }
        //cout << "Probe Finished\n";
        munmap(e2, customer_like_size*sizeof(typename ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry));
        delete HC2;
          
        
    }
    
   
   return result;
   
}
# endif


double query()
{
    
    /*
     * 
     * To save the temporary results, is somehow faster than piplining....
     * 
     */
  /*
    select sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
from customer, "order", orderline
and c_last like 'B%'

where o_w_id = c_w_id
and o_d_id = c_d_id
and o_c_id = c_id

and o_w_id = ol_w_id
and o_d_id = ol_d_id
and o_id = ol_o_id

*/
    uint32_t customer_like_size;

    uint32_t* customer_like = new uint32_t[customer_column_relation.c_id.size()];

    // select customer like %B
    customer_like_size = 0;
    for (uint32_t customer = 0; customer < customer_column_relation.c_last.size(); customer++)
    {
        Varchar <16>& c_last = customer_column_relation.c_last[customer];
        if (*c_last.value == 'B')
        {
           
            customer_like[customer_like_size] = customer;
            customer_like_size++;
        }
    }
    
     
    /*
     *  o_c_id = c_id and c_d_id = o_d_id and o_w_id = c_w_id
     *  PrimaryKey so it's a 1 to N at least
     */
    myarray<std::tuple<uint32_t, uint32_t>> customer_order(order_column_relation.o_c_id.size());
        
    {
        
        ChainingHT<tuple<Integer, Integer, Integer>, uint32_t> *HC = new ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>(customer_like_size);

        ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry* e = static_cast<ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry*>(mmap(NULL, customer_like_size*sizeof(ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
        madvise(e, customer_like_size, MADV_HUGEPAGE);

        parallel_for(blocked_range<size_t>(0, customer_like_size), [&](const blocked_range<size_t>& range) {
            for (size_t i=range.begin(); i!=range.end(); ++i){
                e[i].key = tuple<Integer, Integer, Integer>(
                    customer_column_relation.c_id[customer_like[i]], 
                    customer_column_relation.c_d_id[customer_like[i]], 
                    customer_column_relation.c_w_id[customer_like[i]]
                 );
                    
                e[i].value = customer_like[i];

                HC->insert(e+i);
            }

        });
        
        
        //cout << "Build Finished\n";
        // probe o_c_id

        for (uint32_t i = 0;i < order_column_relation.o_c_id.size();i++) {
            const auto E = HC->lookup(
                tuple<Integer, Integer, Integer>(
                    order_column_relation.o_c_id[i], 
                    order_column_relation.o_d_id[i], 
                    order_column_relation.o_w_id[i]
                 )
              );
            if (E != NULL) {
                //cout << E->value << "||" << i << endl;
                customer_order.insert(make_tuple(E->value,i));
            }
        }
        //cout << "Probe Finished\n";
        munmap(e, customer_like_size*sizeof(typename ChainingHT<tuple<Integer, Integer, Integer>, uint32_t>::Entry));
        delete HC;
          
        
    }
    
        myarray<std::tuple<uint32_t, uint32_t, uint32_t>> customer_order_orderline(orderline_column_relation.ol_d_id.size());
        /*
     *  PrimaryKey so it's a 1 to N at least
     * and o_w_id = ol_w_id
     * and o_d_id = ol_d_id
     * and o_id = ol_o_id
     */    
    {
        
        ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>> *HC = new ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>(customer_order.size);

        ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry* e = static_cast<ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry*>(mmap(NULL, customer_order.size*sizeof(ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
        madvise(e, customer_order.size, MADV_HUGEPAGE);
        // build
        parallel_for(blocked_range<size_t>(0, customer_order.size), [&](const blocked_range<size_t>& range) {
            for (size_t i=range.begin(); i!=range.end(); ++i){
                e[i].key = tuple<Integer, Integer, Integer>(
                    order_column_relation.o_id[get<1>(customer_order.table[i])],
                    order_column_relation.o_d_id[get<1>(customer_order.table[i])], 
                    order_column_relation.o_w_id[get<1>(customer_order.table[i])]
                 );
                    
                e[i].value = customer_order.table[i];

                HC->insert(e+i);
            }

        });
        
        
        // probe o_c_id

        for (uint32_t i = 0;i < orderline_column_relation.ol_o_id.size();i++) {
            const auto E = HC->lookup(
                tuple<Integer, Integer, Integer>(
                    orderline_column_relation.ol_o_id[i], 
                    orderline_column_relation.ol_d_id[i], 
                    orderline_column_relation.ol_w_id[i]
                 )
              );
            if (E != NULL) {
                //cout << E->value << "||" << i << endl;
                customer_order_orderline.insert(std::tuple_cat(E->value,std::make_tuple(i)));
            }
        }
        //cout << "Probe Finished\n";
        munmap(e, customer_order.size*sizeof(typename ChainingHT<tuple<Integer, Integer, Integer>, tuple<Integer, Integer>>::Entry));
        delete HC;
          
        
    }
    
    
    /// sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
    double result = 0;
    for (uint32_t i = 0; i < customer_order_orderline.size; i++)
    {
        auto orderline_line = get<2>(customer_order_orderline.table[i]);
        result += (((double)orderline_column_relation.ol_quantity[orderline_line] *
            ((double)orderline_column_relation.ol_amount[orderline_line]))-
            (((double)customer_column_relation.c_balance[get<0>(customer_order_orderline.table[i])]) *
            ((double)order_column_relation.o_ol_cnt[get<1>(customer_order_orderline.table[i])])));
        
    }
   
   return result;
   
}