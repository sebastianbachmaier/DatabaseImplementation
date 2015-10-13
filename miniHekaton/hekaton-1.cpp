/* Task 0: Mini-Hekaton

This file contains an incomplete implementation of a storage backend
for a relation with three attributes (a, b, and c). Each row is
dynamically allocated on the heap. For attribute a the relation has a
hash index which stores pointers to the rows. Rows with the same hash
value are linked using the next field and can only be accessed through
the hash table (to scan all rows one has to iterate through the hash
table). This storage format is used in Hekaton, the main-memory engine
of Microsoft SQL Server 2014.

Complete the constructor, destructor, insert, lookup, and remove
functions (see TODO). You need a C++11 compiler. In total the code
required is less than 50 lines. The main function contains test code. */

#include <cassert>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Row {
   /// Attribute a
   uint64_t a;
   /// Attribute b
   uint64_t b;
   /// Attribute c
   uint64_t c;
   /// The next pointer for linking rows that have the same hash value in the hash table
   Row* next;
};

struct Relation {
   /// Number of rows in relation
   uint64_t size;
   /// Size of the hash table, must be a power of two
   uint64_t sizeIndex;
   /// Hash table
   Row** index;

   // Construct a relation
   Relation(uint64_t sizeIndex) : size(0), sizeIndex(sizeIndex) {
      // Check that sizeIndex is a power of two
      assert((sizeIndex&(sizeIndex-1))==0);
      /*
      	Using mmap here may be better, everything is initialised with NULL
      	But no "TODO" at the include library...
      */
      index = new Row*[sizeIndex];
      for (int i = 0; i < sizeIndex; ++i)
      {
      	index[i] = NULL;
      }
      
   }

   // Destroy relation (free all memory)
   ~Relation() {
      Row* r;
      Row* r_next;
      for (int i = 0; i < sizeIndex; ++i)
      {
      	r = index[i];
      	while(r)
   		{  	//delete every row in the list ...
   			r_next = r->next;
   			delete r;
   			r = r_next;
 			
   		}
      }
      //... and the table itself
      delete index;
   }

   // Insert a new row
   void insert(uint64_t a,uint64_t b,uint64_t c) {
   		uint64_t h;
    	Row* r = new Row();
    	r->a = a;
    	r->b = b;
    	r->c = c;
    	h = hash(a);
    	/*
    		I suppose there are no concurrent inserts done in this version
			make the next three lines atomic otherwise
		*/
    	r->next = index[h];
    	index[h] = r;
    	this->size++;
   }

   /// Find a row using the index
   Row* lookup(uint64_t a) {
   		uint64_t h = hash(a);
   		Row* r = index[h];
   		/*
   			search in bucket until finding appropriate Row
   		*/
   		while(r)
   		{
   			if(r->a==a)
   			{
   				return r;
   			}
   			r = r->next;
   		}
   		/*
   			no row found: NULL as default
   		*/
   		return NULL;

   }

   // Remove a row
   void remove(Row* row) {
   		if(row==NULL){
   			return;
   		}
   		uint64_t h = hash(row->a);
       	Row* r = index[h];

       	if(r == row)
       	{
       		index[h] = r->next;
       		delete row;
       		return;
       	}

   		while(r->next)
   		{
   			if(r->next==row)
   			{
   				r->next = r->next->next;
   				delete row;
   				return;
   			}
   			r = r->next;
   		}
   }

   // Computes index into hash table for attribute value a
   uint64_t hash(uint64_t a) {
      return a&(sizeIndex-1);
   }
};

void testtable();

int main() {
   uint64_t n=2500000;
   Relation R(1ull<<20);

   // Random test data
   vector<Row> v;
   for (uint64_t i=0; i<n; i++)
      v.push_back({i,i/3,i/7,nullptr});

   {
      random_shuffle(v.begin(),v.end());
      // Insert test data
      auto start=high_resolution_clock::now();
      for (Row& r : v)
         R.insert(r.a,r.b,r.c);
      cout << "insert " << duration_cast<duration<double>>(high_resolution_clock::now()-start).count() << "s" << endl;
   }

   {
      random_shuffle(v.begin(),v.end());
      // Lookup rows
      auto start=high_resolution_clock::now();
      for (Row& r : v) {
         Row* r2=R.lookup(r.a);
         assert(r2&&(r2->a==r.a));
      }
      cout << "lookup " << duration_cast<duration<double>>(high_resolution_clock::now()-start).count() << "s" << endl;
   }

   {
      auto start=high_resolution_clock::now();
      // Scan all entries and add attribute a
      uint64_t sum=0;
      for (uint64_t i=0; i<R.sizeIndex; i++) {
         Row* r=R.index[i];
         while (r) {
            sum+=r->a;
            r=r->next;
         }
      }
      cout << "scan " << duration_cast<duration<double>>(high_resolution_clock::now()-start).count() << "s" << endl;
      assert(sum==((n*(n-1))/2));
   }

   {
      random_shuffle(v.begin(),v.end());
      // Delete all entries
      auto start=high_resolution_clock::now();
      for (Row& r : v) {
         Row* r2=R.lookup(r.a);
         assert(r2);
         R.remove(r2);
         assert(!R.lookup(r.a));
      }
      cout << "remove " << duration_cast<duration<double>>(high_resolution_clock::now()-start).count() << "s" << endl;
      // Make sure the table is empty
      for (unsigned i=0; i<R.sizeIndex; i++)
         assert(R.index[i]==nullptr);
   }

   return 0;
}

#if 0
/*
	some minor testing
*/
void testtable(){
	Relation R(1ull<<5);
	int size = 100;

	for (int i = 0; i < size; ++i)
	{
		R.insert(i,i/2,i/3);
	}

	for (int i = 0; i < R.sizeIndex; ++i)
	{
		Row* r = R.index[i];
		cout << i << ":||->";
		while(r)
		{
			cout << "[" << r->a << "," << r->b << "," << r->c << "] -> ";
			r = r->next;
		}

		cout << "[NULL]" << endl;
	}

	Row* r = R.lookup(50);
	cout << endl << "lookup:" << endl;
	cout << "[" << r->a << "," << r->b << "," << r->c << "]" << endl;
	cout << endl << "remove..." << endl;
	R.remove(r);
	for (int i = 0; i < R.sizeIndex; ++i)
	{
		Row* r = R.index[i];
		cout << i << ":||->";
		while(r)
		{
			cout << "[" << r->a << "," << r->b << "," << r->c << "] -> ";
			r = r->next;
		}

		cout << "[NULL]" << endl;
	}
}
#endif

