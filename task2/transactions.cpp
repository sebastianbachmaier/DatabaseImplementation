#include <iostream>
#include <chrono>
#include "schema_generated.hpp"
#include "oltp.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace std::chrono;

void oltp(Timestamp now) {
   deliveryRandom(now);
   int rnd=urand(1,100);
   if (rnd<=10) {
      //deliveryRandom(now);
      //cout << "worked" << endl;
   } else {
      //newOrderRandom(now);
      
   }
}

void newOrder (int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime)
{
    // select w_tax from warehouse w where w.w_id=w_id;
    Numeric<4, 4> w_tax = warehouse_row_relation[warehouse_primary_key[w_id]].w_tax;
    
    // select c_discount from customer c where c_w_id=w_id and c_d_id=d_id and c.c_id=c_id;
    Numeric<4, 4>  c_discount = customer_row_relation[customer_primary_key[w_id][d_id][c_id]].c_discount;
    
    // select d_next_o_id as o_id,d_tax from district d where d_w_id=w_id and d.d_id=d_id;
    Integer o_id = district_row_relation[district_primary_key[w_id][d_id]].d_next_o_id;
    Numeric<4, 4> d_tax = district_row_relation[district_primary_key[w_id][d_id]].d_tax;
    
    //update district set d_next_o_id=o_id+1 where d_w_id=w_id and district.d_id=d_id;
    district_row_relation[district_primary_key[w_id][d_id]].d_next_o_id = o_id+(Integer)1;
    
    
    //var integer all_local = 1;
    Integer all_local = 1;
    
    // forsequence (index between 0 and items-1) 
    for (int32_t index = 0; index <= items-1; index++)
    {
        // if (w_id<>supware[index])
        //    all_local=0;
        if (w_id != supware[index])
        {
            all_local = 0;
        }  
    }
    
    // insert into "order" values (o_id,d_id,w_id,c_id,datetime,0,items,all_local);
    order element0 {o_id, d_id, w_id, c_id, datetime, 0, items, all_local};
    order_insert(element0);
    
    // insert into neworder values (o_id,d_id,w_id)
    neworder element1 {o_id, d_id, w_id};
    neworder_insert(element1);
    
    //  forsequence (index between 0 and items-1) 
    for (int32_t index = 0; index <= items-1;index++)
    {
        //select i_price from item where i_id=itemid[index];
        Numeric<5, 2> i_price = item_row_relation[item_primary_key[itemid[index]]].i_price;
        
        //  select s_quantity,s_remote_cnt,s_order_cnt,case d_id when 1 then s_dist_01 when 2 then s_dist_02 when 3 then s_dist_03 when 4 then s_dist_04 when 5 then s_dist_05 when 6 then s_dist_06 when 7 then s_dist_07 when 8 then s_dist_08 when 9 then s_dist_09 when 10 then s_dist_10 end as s_dist 
        //  from stock where s_w_id=supware[index] and s_i_id=itemid[index];
       
        stock& result0 = stock_row_relation[stock_primary_key[supware[index]][itemid[index]]];
        Numeric<4,0> s_quantity = result0.s_quantity;
        Numeric<4,0> s_remote_cnt = result0.s_remote_cnt;
        Numeric<4,0> s_order_cnt = result0.s_order_cnt;
        Char<24> s_dist;
        switch ( d_id )
        {
        case 1:
            s_dist =  result0.s_dist_01;
            break;
        case 2:
            s_dist =  result0.s_dist_02;
            break;
        case 3:
            s_dist =  result0.s_dist_03;
            break;
        case 4:
            s_dist =  result0.s_dist_04;
            break;
        case 5:
            s_dist =  result0.s_dist_05;
            break;
        case 6:
            s_dist =  result0.s_dist_06;
            break;
        case 7:
            s_dist =  result0.s_dist_07;
            break;
        case 8:
            s_dist =  result0.s_dist_08;
            break;
        case 9:
            s_dist =  result0.s_dist_09;
            break;
        case 10:
            s_dist =  result0.s_dist_10;
            break;
        }
        
      // if (s_quantity>qty[index]) {
      if ((int32_t)s_quantity>qty[index]) {
          // update stock set s_quantity=s_quantity-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
          stock_row_relation[stock_primary_key[supware[index]][itemid[index]]].s_quantity = s_quantity-(Numeric<4, 0>)qty[index];
      }else{
          // update stock set s_quantity=s_quantity+91-qty[index] where s_w_id=supware[index] and s_i_id=itemid[index];
          stock_row_relation[stock_primary_key[supware[index]][itemid[index]]].s_quantity =  s_quantity+(Numeric<4, 0>)91-(Numeric<4, 0>)qty[index];
      }
      
      //if (supware[index]<>w_id) {
      if (supware[index] != w_id) {
         //update stock set s_remote_cnt=s_remote_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
         stock_row_relation[stock_primary_key[w_id][itemid[index]]].s_remote_cnt = s_remote_cnt+(Numeric<4, 0>)1;
      } else {
         //update stock set s_order_cnt=s_order_cnt+1 where s_w_id=w_id and s_i_id=itemid[index];
         stock_row_relation[stock_primary_key[w_id][itemid[index]]].s_order_cnt = s_order_cnt+(Numeric<4, 0>)1;
      }
      
      // var numeric(6,2) ol_amount=qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
      Numeric<6,2> ol_amount = qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
      
      // insert into orderline values (o_id,d_id,w_id,index+1,itemid[index],supware[index],0,qty[index],ol_amount,s_dist);
      orderline element2 {o_id, d_id, w_id, index+1, itemid[index], supware[index], 0,qty[index],ol_amount,s_dist};
      orderline_insert(element2);
    }
    
}


void delivery(int32_t w_id, int32_t o_carrier_id, Timestamp datetime)
{
    for (unsigned d_id = 1; d_id <= 10; d_id++)
    {
        // select min(no_o_id) as o_id from neworder where no_w_id=w_id and no_d_id=d_id order by no_o_id else { continue; } -- ignore this district if no row found
        std::map<Integer,uint64_t>& result0 = neworder_primary_key[w_id][d_id];
        if (result0.empty())
        {
            continue;
        }
        //  min(no_o_id) as o_id
        cout << "before " << result0.begin()->first << " " <<  result0.begin()->second <<  " " << d_id << " " << w_id << endl;
        Integer o_id = neworder_row_relation.at(result0.begin()->second).no_o_id;
        cout << "after" << endl;
        // delete from neworder where no_w_id=w_id and no_d_id=d_id and no_o_id=o_id;
        uint64_t to_delete = neworder_primary_key[w_id][d_id][o_id];
        neworder_delete(to_delete);
        
        // select o_ol_cnt,o_c_id from "order" o where o_w_id=w_id and o_d_id=d_id and o.o_id=o_id;
        uint64_t result1 = order_primary_key[w_id][d_id][o_id];
        cout << "result1 " <<  endl;
        Numeric<2,0> o_ol_cnt = order_row_relation[result1].o_ol_cnt;
        Integer o_c_id = order_row_relation[result1].o_c_id;
        
        
        //update "order" set o_carrier_id=o_carrier_id where o_w_id=w_id and o_d_id=d_id and "order".o_id=o_id;
        //uint64_t result2 = order_primary_key[w_id][d_id][o_id]; //use same result again
        order_row_relation[result1].o_carrier_id = o_carrier_id; 
        
        // var numeric(6,2) ol_total=0;
        Numeric<6, 2> ol_total = 0;
        
        //forsequence (ol_number between 1 and o_ol_cnt) {
        for (unsigned ol_number = 0; ol_number <=  o_ol_cnt; ol_number++)
        {
            // select ol_amount from orderline where ol_w_id=w_id and ol_d_id=d_id and ol_o_id=o_id and orderline.ol_number=ol_number;
            uint64_t result3 = orderline_primary_key[w_id][d_id][o_id][ol_number];
            cout << "result3 " <<  endl;
             Numeric<6, 2> ol_amount = orderline_row_relation[result3].ol_amount;
            // ol_total=ol_total+ol_amount;
            ol_total = ol_total+ol_amount;
            // update orderline set ol_delivery_d=datetime where ol_w_id=w_id and ol_d_id=d_id and ol_o_id=o_id and orderline.ol_number=ol_number;
            //uint64_t result4 = orderline_primary_key[w_id][d_id][o_id][ol_number]; //use same result again
            orderline_row_relation[result3].ol_delivery_d = datetime;
        }
        
        //update customer set c_balance=c_balance+ol_total where c_w_id=w_id and c_d_id=d_id and c_id=o_c_id;
        unsigned result5 = customer_primary_key[w_id][d_id][o_c_id];
        cout << "result5 " <<  endl;
         customer_row_relation[result5].c_balance += (double)ol_total;
    }
};




int main(int argc,  char** argv)
{
    srand (time(NULL));
    
    
    cout <<  "loading tbl..." << endl;
    init_tbl();
 
    
    cout <<  "order(" << order_row_relation.size() << ") orderline(" << orderline_row_relation.size() << ") neworder(" << neworder_row_relation.size() << ")" << endl;
    auto start=high_resolution_clock::now();
    
    for (int i = 0; i < 4300; i++)
    {
         oltp(time(NULL));
    }
    
     for (int i = 0; i < 100; i++)
    {
         oltp(time(NULL));
    }
    
    cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
    cout <<  "order(" << order_row_relation.size() << ") orderline(" << orderline_row_relation.size() << ") neworder(" << neworder_row_relation.size() << ")" << endl;
    
    
    return 0;
}