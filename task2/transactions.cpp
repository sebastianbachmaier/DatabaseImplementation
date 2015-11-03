#include <iostream>
#include <chrono>
#include "schema_generated.hpp"
#include "oltp.hpp"
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>
#include <sys/mman.h>
#include <math.h>
#include <mutex>
#include <stdint.h>
#include <stdint-gcc.h>

using namespace std;
using namespace tbb;
using namespace std::chrono;


void oltp(Timestamp now) {
   //deliveryRandom(now);
   int rnd=urand(1,100);
   if (rnd<=10) {
      deliveryRandom(now);
      ////cout << "worked" << endl;
   } else {
      newOrderRandom(now);
      
   }
}


namespace std{
    namespace
    {

        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     http://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, get<0>(tuple));
          }
        };
    }
    
    template <typename ... TT>
    struct hash<std::tuple<TT...>> 
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {                                              
            size_t seed = 0;                             
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
            return seed;                                 
        }                                              

    };
    template <>
    struct hash<Integer> 
    {
        size_t
        operator()(Integer const& tt) const
        {                                              
            return std::hash<int32_t>()(tt.value);                                 
        }                                              

    };
    
}


template <typename K, typename T>
class ChainingHT {
   public:
   // Chained tuple entry
   struct Entry {
      K key;
      T value;
      Entry* next;
   };
   struct Bucket {
      std::atomic<Entry*> head;
   };
   
   inline uint64_t hashKey(K u) {
   // MurmurHash64A
   std::hash<K> keyhash;
   uint64_t k =  keyhash(u);
   const uint64_t m = 0xc6a4a7935bd1e995;
   const int r = 47;
   uint64_t h = 0x8445d61a4e774912 ^ (8*m);
   k *= m;
   k ^= k >> r;
   k *= m;
   h ^= k;
   h *= m;
   h ^= h >> r;
   h *= m;
   h ^= h >> r;
   return h|(1ull<<63);
}
   
   uint64_t mask;
   Bucket* table;

   // Constructor
   ChainingHT(uint64_t size) {
      mask=pow(2,((unsigned)log2(size))+2)-1;
      uint64_t rsize = (mask+1)*sizeof(Bucket);
      table=static_cast<Bucket*>(mmap(NULL, rsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
      madvise(table, size, MADV_HUGEPAGE);
   }

   // Destructor
   ~ChainingHT() {
      munmap(table, ((mask+1)*sizeof(Bucket)));
   }

   inline Entry* lookup(K key) {
      uint64_t hash = hashKey(key);
      uint64_t pos = hash&mask;
      Entry *current = table[pos].head;
      while(current != NULL){
         if(current->key == key){
            return current;
         }
         current = current->next;
      };
      return NULL;
   }

   inline uint64_t multilookup(uint64_t key){
      //cout << "new lookup" << endl;
      uint64_t hash = hashKey(key);
      uint64_t pos = hash&mask;
      Entry *current = table[pos].head;
      uint64_t res = 0;
      while(current != NULL){
         if(current->key == key){
            res++;
         }
         current = current->next;
      }
      return res;
   }
   
   inline bool exists(K key){
      //cout << "new lookup" << endl;
      uint64_t hash = hashKey(key);
      uint64_t pos = hash&mask;
      Entry *current = table[pos].head;
      while(current != NULL){
         if(current->key == key){
            return true;
         }
         current = current->next;
      }
      return false;
   }

   inline void insert(Entry* entry) {
      uint64_t hash = hashKey(entry->key);
      uint64_t pos = hash&mask;
      entry->next = table[pos].head.exchange(entry);
      //std::cout << table[pos].head << "   " << entry << "   " << entry->next << std::endl;
   }
};

template <typename T>
class atomicarray {
   public:
   // Entry
   T* table;
   int32_t max_size;
   //tbb::atomic<int32_t> size;
   int32_t size;
   // Constructor
   atomicarray(uint64_t s) :max_size(s),  size(0) {
      table=(T*)malloc(sizeof(T) *max_size);
   }

   // Destructor
   ~atomicarray() {
      free(table);
   }

 
   inline void insert(T key) {
      table[size] = key;
      size++;
   }

};

uint64_t query()
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


select sum(ol_quantity*ol_amount-c_balance*o_ol_cnt)
from customer, "order", orderline

and c_last like 'B%'

and o_c_id = c_id

where o_w_id = c_w_id
and o_w_id = ol_w_id

and o_d_id = c_d_id
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
    atomicarray<std::tuple<uint32_t, uint32_t>> customer_order(order_column_relation.o_c_id.size());
        
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
    
        atomicarray<std::tuple<uint32_t, uint32_t, uint32_t>> customer_order_orderline(orderline_column_relation.ol_d_id.size());
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
    uint64_t result = 0;
    for (uint32_t i = 0; i <= customer_order_orderline.size; i++)
    {
        auto orderline_line = get<2>(customer_order_orderline.table[i]);
        result += ((orderline_column_relation.ol_quantity[orderline_line] *
            orderline_column_relation.ol_amount[orderline_line])-
            (customer_column_relation.c_balance[get<0>(customer_order_orderline.table[i])] *
            order_column_relation.o_ol_cnt[get<1>(customer_order_orderline.table[i])]));
        
    }
   
   return result;
   
}


void newOrder (int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime)
{

    auto w_tax = warehouse_column_relation.w_tax.at(warehouse_primary_key.at(warehouse_primary_key_t(w_id)));
    auto c_discount = customer_column_relation.c_discount.at(customer_primary_key.at(customer_primary_key_t(w_id, d_id, c_id)));
    uint64_t& result0 = district_primary_key.at(district_primary_key_t(w_id, d_id));
    auto d_tax = district_column_relation.d_tax.at(result0);
    auto o_id = district_column_relation.d_next_o_id.at(result0);
    district_column_relation.d_next_o_id.at(result0) = (int)o_id+1;
    
    auto all_local = 1;
    for (auto index = 0; index <= items-1;index++) {
      if (w_id != supware[index])
         all_local=0;
    }
    
    order_column_insert(order {o_id,d_id,w_id,c_id,datetime,0,items,all_local} );
    neworder_column_insert(neworder {o_id,d_id,w_id} );
  
    for (auto index = 0; index <= items-1; index++){
        auto i_price = item_column_relation.i_price.at(item_primary_key.at(item_primary_key_t(itemid[index])));
        uint64_t& result1 = stock_primary_key.at(stock_primary_key_t(supware[index],itemid[index]));
        auto& s_quantity = stock_column_relation.s_quantity.at(result0);
        auto s_remote_cnt = stock_column_relation.s_remote_cnt.at(result0);
        auto s_order_cnt = stock_column_relation.s_order_cnt.at(result0);
        Char<24> s_dist;
        switch ( d_id )
        {
        case 1:
            s_dist =  stock_column_relation.s_dist_01.at(result0);
            break;
        case 2:
            s_dist =  stock_column_relation.s_dist_02.at(result0);
            break;
        case 3:
            s_dist =  stock_column_relation.s_dist_03.at(result0);
            break;
        case 4:
            s_dist =  stock_column_relation.s_dist_04.at(result0);
            break;
        case 5:
            s_dist =  stock_column_relation.s_dist_05.at(result0);
            break;
        case 6:
            s_dist =  stock_column_relation.s_dist_06.at(result0);
            break;
        case 7:
            s_dist =  stock_column_relation.s_dist_07.at(result0);
            break;
        case 8:
            s_dist =  stock_column_relation.s_dist_08.at(result0);
            break;
        case 9:
            s_dist =  stock_column_relation.s_dist_09.at(result0);
            break;
        case 10:
            s_dist =  stock_column_relation.s_dist_10.at(result0);
            break;
        }
        if ((int32_t)s_quantity>qty[index]) {
            s_quantity = s_quantity - (Numeric<4,0>)qty[index];
        } else {
            s_quantity = s_quantity + (Numeric<4,0>)(91-qty[index]);
        }

        uint64_t& result2 = stock_primary_key.at(stock_primary_key_t(w_id,itemid[index]));
        if (supware[index] != w_id) {
            stock_column_relation.s_remote_cnt.at(result2) = s_remote_cnt+(Numeric<4,0>)1;
        } else {
            stock_column_relation.s_order_cnt.at(result2) = s_order_cnt+(Numeric<4,0>)1;
        }
        
        auto ol_amount=qty[index]*i_price*(1.0+w_tax+d_tax)*(1.0-c_discount);
        orderline_column_insert(orderline {o_id,d_id,w_id,index+1,itemid[index],supware[index],0,qty[index],ol_amount,s_dist});
    }

   
}

void delivery(int32_t w_id, int32_t o_carrier_id, Timestamp datetime)
{


    for (int32_t d_id =0; d_id <= 10; d_id++) {
        auto result0 = neworder_primary_key.lower_bound(neworder_primary_key_t(w_id, d_id, numeric_limits<Integer>::min()) );
        /// check if minimum still is in range
        if ((int32_t)get<0>(result0->first) != w_id || (int32_t)get<1>(result0->first) != d_id)
            continue;
        auto o_id = neworder_column_relation.no_o_id.at(result0->second);
        neworder_column_delete(result0->second);
        
        uint64_t result1 = order_primary_key.at(order_primary_key_t(w_id, d_id, o_id));
        auto o_ol_cnt = order_column_relation.o_ol_cnt.at(result1);
        auto o_c_id = order_column_relation.o_c_id.at(result1);
        order_column_relation.o_carrier_id.at(result1) = o_carrier_id;
        Numeric<6, 2> ol_total = 0;
        for (int32_t ol_number = 1; ol_number <= o_ol_cnt; ol_number++) {
            uint64_t result2 = orderline_primary_key.at(orderline_primary_key_t(w_id, d_id, o_id, ol_number));
            auto ol_amount = orderline_column_relation.ol_amount.at(result2);
            ol_total=ol_total+ol_amount;
            orderline_column_relation.ol_delivery_d.at(result2) = datetime; 
        }
        uint64_t result3 = customer_primary_key.at(customer_primary_key_t(w_id, d_id, o_c_id));
        auto& c = customer_column_relation.c_balance.at(result3);
        c = c + ol_total;
        
    }
}

int main(int argc,  char** argv)
{
    

    
    srand (time(NULL));
    
    auto start=high_resolution_clock::now();
    cout <<  "loading tbl..." << endl;
    init_tbl();
    cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
        
    for (int i = 0; i < 10; i++)
    {
        
        start=high_resolution_clock::now();
        uint64_t result = query();
        cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
        cout << "Result: " << result << endl;
        
    }
    
 
    return 0;
    cout <<  "order(" << order_column_relation.o_id.size() << ") orderline(" << orderline_column_relation.ol_i_id.size() << ") neworder(" << neworder_column_relation.no_w_id.size() << ")" << endl;
    start=high_resolution_clock::now();
    
    for (int i = 0; i < 1000000; i++)
    {
         oltp(time(NULL));
    }
    
    
    cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
    cout <<  "order(" << order_column_relation.o_id.size() << ") orderline(" << orderline_column_relation.ol_i_id.size() << ") neworder(" << neworder_column_relation.no_w_id.size() << ")" << endl;
    
    
    return 0;
}