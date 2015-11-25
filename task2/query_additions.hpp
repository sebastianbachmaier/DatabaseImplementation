#include <iostream>
#include <array>
#include <atomic>

#define MaxThreads 20

class conc
{
public:   
        
           
      struct list {
        std::stringstream* str;
        list* next;
          
      };
    
      std::atomic<list*> head;

      
      std::stringstream* conc_add()
      {
          list* l = new list;
          l->str = new stringstream;
          l->next = head.exchange(l);
          return l->str;
      }
      
      conc()
      {
        //for (unsigned i = 0; i < maxN+3; i++)
        //    cout[i] = new stringstream;
        //currentT = 0;
        head = NULL;
      }
      
      std::string show()
      {
            std::stringstream out;
            list* l = head;
            while (l != NULL)
            {
               
                out <<   l->str->str();
                l = l->next;   
            }
            
            return out.str();
      }
      
      ~conc()
      {
        while (head != NULL)
        {
         list* n = head;
         list* l = head.exchange(n->next);   
         delete l->str;
         delete l;
        }
      }
      
};


