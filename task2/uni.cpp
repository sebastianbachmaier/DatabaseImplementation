#include <iostream>
#include <chrono>
#include "uni_schema_generated.hpp"
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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


using namespace std;
using namespace tbb;
using namespace std::chrono;

#include "uni_query_generated.cpp"

int main ( int argc,  char** argv )
{
    srand ( time ( NULL ) );
    
    cout <<  "loading tbl..." << endl;
    auto start=high_resolution_clock::now();
    DATABASE db;
    db.init_tbl();
    cout << "Time: " << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << "s" << endl;
    
    std::cout << query(&db);

    return 0;
}
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
