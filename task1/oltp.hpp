#include <cstdlib>
#include "Types.hpp"
#include "functions.hpp"
#include "neworder.hpp"



Integer warehouses = 5;

   


Varchar<20> genName(Integer id)
{
	Varchar<5> parts[10];
	parts[0].build("BAR");
	parts[1].build("OUGHT");
	parts[2].build("ABLE");
	parts[3].build("PRI");
	parts[4].build("PRES");
	parts[5].build("ESE");
	parts[6].build("ANTI");
	parts[7].build("CALLY");
	parts[8].build("ATION");
	parts[9].build("EING");

	Integer n[3];
	n[0] = (id/100)%10;
	n[1] = (id/10)%10;
	n[2] = id%10;
	return parts[n[0].value]+parts[n[1].value]+parts[n[2].value];
};

Numeric<6,2> randomnumeric62(Numeric<6,2> min,Numeric<6,2> max)
{
    //return min + ( (cast (rand()%cast ((max-min)*100) as Integer) as Numeric<6,2>)/100);
   
	Integer i = (Integer)(max-min)*(Integer)100;
	Numeric<6,2> n = (rand()% i);
	
    return min + ( n/(Integer)100);
};

Integer nurand(Integer A,Integer x,Integer y)
{
   return ((((rand()%A)|(rand()%(y-x+(Integer)1)+x))+(Integer)42)%(y-x+(Integer)1))+x;
};

Integer urand(Integer min,Integer max)
{
   return (rand()%(max-min+(Integer)1))+min;
};

Integer urandexcept(Integer min,Integer max,Integer v)
{
   if (max<=min)
      return min;
   Integer r=(rand()%(max-min))+min;
   if (r>=v)
      return r+(Integer)1; else
      return r;
};

void newOrderRandom(Timestamp now,Integer w_id)
{
   Integer d_id=urand(1,10);
   Integer c_id=nurand(1023,1,3000);
   Integer ol_cnt=urand(5,15);

   Integer supware[15];
   Integer itemid[15];
   Integer qty[15];
   for (int index =  0; index < ol_cnt; index++) {
      if (urand(1,100)>(Integer)1)
         supware[index]=w_id; else
         supware[index]=urandexcept(1,warehouses,w_id);
      itemid[index]=nurand(8191,1,100000);
      qty[index]=urand(1,10);
   }
   for(int index = ol_cnt; index <= 14; index++) {
      supware[index]=w_id;
   }

   newOrder(w_id,d_id,c_id,ol_cnt,supware,itemid,qty,now);
};

void paymentRandom(Timestamp now,Integer w_id)
{
   Integer d_id=urand(1,10);
   Integer c_w_id;
   Integer c_d_id;
   if (urand(1,100)<=(Integer)85) {
      c_w_id=w_id;
      c_d_id=d_id;
   } else {
      c_w_id=urandexcept(1,warehouses,w_id);
      c_d_id=urand(1,10);
   }
   Numeric<6,2> h_amount=randomnumeric62(1.00,5000.00);
   Timestamp h_date=now;

   if (urand(1,100)<=(Integer)60) {
      paymentByName(w_id,d_id,c_w_id,c_d_id,genName(nurand(255,0,999)),h_date,h_amount,now);
   } else {
      paymentById(w_id,d_id,c_w_id,c_d_id,nurand(1023,1,3000),h_date,h_amount,now);
   }
};

void ostatRandom(Integer w_id)
{
   Integer d_id=urand(1,10);

   if (urand(1,100)<=(Integer)60) {
      ostatByName(w_id,d_id,genName(nurand(255,0,999)));
   } else {
      ostatById(w_id,d_id,nurand(1023,1,3000));
   }
};

void deliveryRandom(Timestamp now,Integer w_id)
{
   Integer carrier_id=urand(1,10);
   delivery(w_id,carrier_id,now);
};

void slevRandom(Integer w_id)
{
   Integer d_id=urand(1,10);
   Integer threshold=urand(10,20);

   slev(w_id,d_id,threshold);
};

Integer partitionedOltp(Timestamp now,Integer w_id)
{
   Integer choice=urand(1,1000);

   // Payment?
   if (choice<=(Integer)430) {
      paymentRandom(now,w_id);
      return (Integer)1;
   }
   choice=choice-(Integer)430;

   // Order status?
   if (choice<=(Integer)40) {
      ostatRandom(w_id);
      return (Integer)2;
   }
   choice=choice-(Integer)40;

   // Delivery?
   if (choice<=(Integer)45) {
      deliveryRandom(now,w_id);
      return (Integer)3;
   }
   choice=choice-(Integer)45;

   // Stock level?
   if (choice<=(Integer)40) {
      slevRandom(w_id);
      return (Integer)40;
   }

   // Default to new order
   newOrderRandom(now,w_id);
   return (Integer)0;
};

Integer oltp(Timestamp now)
{
   return partitionedOltp(now,urand(1,warehouses));
};
