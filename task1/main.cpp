#include <iostream>
#include "schema.hpp"


int main(int argc, char **argv) {
	
	warehouse* w = new warehouse;
	
	std::cout << w->w_id << std::endl;
	
	delete w;
	return 0;
}

