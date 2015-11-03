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

using namespace tbb;
using namespace std;

inline uint64_t hashKey(uint64_t k) {
   // MurmurHash64A
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

class ChainingLockingHT {
   public:
   // Chained tuple entry
   struct Entry {
      uint64_t key;
      uint64_t value;
      Entry* next;
   };

   struct Bucket {
      Entry* head;
      std::mutex m;
   };

   uint64_t mask;
   Bucket* table;

   // Constructor
   ChainingLockingHT(uint64_t size) {
      mask=pow(2,((unsigned)log2(size))+2)-1;
      uint64_t rsize = (mask+1)*sizeof(Bucket);
      table=static_cast<Bucket*>(mmap(NULL, rsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
      madvise(table, size, MADV_HUGEPAGE);
   }

   // Destructor
   ~ChainingLockingHT() {
      munmap(table, ((mask+1)*sizeof(Bucket)));
   }

   inline Entry* lookup(uint64_t key) {
      uint64_t hash = hashKey(key);
      uint64_t pos = hash & mask;
      Entry *current = table[pos].head;
      do{
         if(current->key == key){
            return current;
         }
      }while(current = current->next);
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

   inline void insert(Entry* entry) {
      uint64_t hash = hashKey(entry->key);
      uint64_t pos = hash & mask;
      table[pos].m.lock();
      entry->next = table[pos].head;
      table[pos].head = entry;
      table[pos].m.unlock();
   }
};


class LinearProbingHT {
   public:
   // Entry
   struct Entry {
      uint64_t key;
      uint64_t value;
      std::atomic<bool> marker;
   };
   uint64_t mask;
   Entry* table;

   // Constructor
   LinearProbingHT(uint64_t size) {
      mask=pow(2,((unsigned)log2(size))+2)-1;
      uint64_t rsize = (mask+1)*sizeof(Entry);
      table=static_cast<Entry*>(mmap(NULL, rsize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
      madvise(table, size, MADV_HUGEPAGE);
   }

   // Destructor
   ~LinearProbingHT() {
      munmap(table, ((mask+1)*sizeof(Entry)));
   }

   inline Entry* lookup(uint64_t key) {
      uint64_t hash = hashKey(key);
      uint64_t pos = hash & mask;
      while(table[pos].marker){
         if(key == table[pos].key){
            return table+pos;
         }else{
            pos = (pos+1) & mask;
         }
      }
      return NULL;
   }

   inline uint64_t multilookup(uint64_t key) {
      uint64_t result = 0;
      uint64_t hash = hashKey(key);
      uint64_t pos = hash & mask;
      while(table[pos].marker){
         if(key == table[pos].key){
            result++;
         }
         pos = (pos+1) & mask;
      }
      return result;
   }

   inline void insert(uint64_t key) {
      bool expected = false;
      uint64_t hash = hashKey(key);
      uint64_t pos = hash & mask;
      while(!table[pos].marker.compare_exchange_strong(expected, true)){
         expected = false;
         pos = (pos+1) & mask;
      }
      table[pos].key = key;
      table[pos].value = key;
   }
   

};
#if 0
int main(int argc,char** argv) {
   uint64_t sizeR = atoi(argv[1]);
   uint64_t sizeS = atoi(argv[2]);
   unsigned threadCount = atoi(argv[3]);

   task_scheduler_init init(threadCount);

   // Init build-side relation R with random data
   uint64_t* R=static_cast<uint64_t*>(malloc(sizeR*sizeof(uint64_t)));
   parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
         unsigned int seed=range.begin();
         for (size_t i=range.begin(); i!=range.end(); ++i)
            R[i]=rand_r(&seed)%sizeR;
      });

   // Init probe-side relation S with random data
   uint64_t* S=static_cast<uint64_t*>(malloc(sizeS*sizeof(uint64_t)));
   parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
         unsigned int seed=range.begin();
         for (size_t i=range.begin(); i!=range.end(); ++i)
            S[i]=rand_r(&seed)%sizeR;
      });

   // STL
   {
      // Build hash table (single threaded)
      tick_count buildTS=tick_count::now();
      unordered_multimap<uint64_t,uint64_t> ht(sizeR);
      for (uint64_t i=0; i<sizeR; i++)
         ht.emplace(R[i],0);
      tick_count probeTS=tick_count::now();
      cout << "STL      build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

      // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
            for (size_t i=range.begin(); i!=range.end(); ++i) {
               auto range=ht.equal_range(S[i]);
               for (unordered_multimap<uint64_t,uint64_t>::iterator it=range.first; it!=range.second; ++it)
                  localHitCounter++;
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
   }

   // Locked Chaining
   {
      tick_count buildTS = tick_count::now();
      ChainingLockingHT *LC = new ChainingLockingHT(sizeR);

      ChainingLockingHT::Entry *e = static_cast<ChainingLockingHT::Entry*>(mmap(NULL, sizeR*sizeof(ChainingLockingHT::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
      madvise(e, sizeR, MADV_HUGEPAGE);

      parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
          for (size_t i=range.begin(); i!=range.end(); ++i){
             e[i].key = R[i];
             e[i].value = R[i];
             LC->insert(e+i);
          }

      });
      tick_count probeTS=tick_count::now();
      cout << "LC      build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

      // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
          uint64_t localHitCounter=0;
          for (size_t i=range.begin(); i!=range.end(); ++i) {
             localHitCounter += LC->multilookup(S[i]);
          }
          hitCounter+=localHitCounter;
      });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
      << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
      << "count: " << hitCounter << endl;

      munmap(e, (sizeR)*sizeof(ChainingLockingHT::Entry));
   }

   // Lock-free Chaining
   {
      tick_count buildTS = tick_count::now();
      ChainingHT *HC = new ChainingHT(sizeR);

      ChainingHT::Entry *e = static_cast<ChainingHT::Entry*>(mmap(NULL, sizeR*sizeof(ChainingHT::Entry), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
      madvise(e, sizeR, MADV_HUGEPAGE);

      parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
          for (size_t i=range.begin(); i!=range.end(); ++i){
             e[i].key = R[i];
             e[i].value = R[i];
             HC->insert(e+i);
          }

      });
      tick_count probeTS=tick_count::now();
      cout << "HC      build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

      // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
          uint64_t localHitCounter=0;
          for (size_t i=range.begin(); i!=range.end(); ++i) {
             localHitCounter += HC->multilookup(S[i]);
          }
          hitCounter+=localHitCounter;
      });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
      << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
      << "count: " << hitCounter  << endl;
      munmap(e, (sizeR)*sizeof(ChainingHT::Entry));
   }


   // Lock-free Linear Probing
   {
      tick_count buildTS = tick_count::now();
      LinearProbingHT *LP = new LinearProbingHT(sizeR);

      parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t>& range) {
        for (size_t i=range.begin(); i!=range.end(); ++i)
            LP->insert(R[i]);
      });
      tick_count probeTS=tick_count::now();
      cout << "LP      build:" << (sizeR/1e6)/(probeTS-buildTS).seconds() << "MT/s ";

      // Probe hash table and count number of hits
      std::atomic<uint64_t> hitCounter;
      hitCounter=0;
      parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t>& range) {
            uint64_t localHitCounter=0;
            for (size_t i=range.begin(); i!=range.end(); ++i) {
               localHitCounter += LP->multilookup(S[i]);
            }
            hitCounter+=localHitCounter;
         });
      tick_count stopTS=tick_count::now();
      cout << "probe: " << (sizeS/1e6)/(stopTS-probeTS).seconds() << "MT/s "
           << "total: " << ((sizeR+sizeS)/1e6)/(stopTS-buildTS).seconds() << "MT/s "
           << "count: " << hitCounter << endl;
   }
   return 0;
}
# endif

