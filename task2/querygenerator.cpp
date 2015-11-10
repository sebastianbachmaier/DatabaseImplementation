#include <iostream>
#include <memory>
#include <fstream>
#include <map>
#include "Parser.hpp"
#include "operator.hpp"
#include "Schema.hpp"
#define in :

#define fori(a, b) for (auto a = 0; a < b; i++) 


int main(void)
{
     
    const char* c = "../tpc-c/schema.sql";

    Parser p ( c );
    try
    {
       
        std::unique_ptr<Schema> schema;
        schema = p.parse();
        QueryTree tree("query4");
        
       
        QueryTree::Operator* scan2 = tree.insert(new QueryTree::Operator(6, QueryTree::Type::tablescan, schema->findRelation("order"))); 
        QueryTree::Operator* scan1 = tree.insert(new QueryTree::Operator(5, QueryTree::Type::tablescan, schema->findRelation("customer"))); 
        QueryTree::Operator* sele0 = tree.insert(new QueryTree::Operator(4, QueryTree::Type::selection, {"c_id", "c_w_id", "c_d_id"},"c_id == Integer(322) && c_w_id == Integer(1) && c_d_id == Integer(1)", {scan1})); 
        QueryTree::Operator* join1 = tree.insert(new QueryTree::Operator(3, QueryTree::Type::join, {"o_w_id", "c_w_id", "o_d_id", "c_d_id", "o_c_id", "c_id"}, "o_w_id == c_w_id && o_d_id == c_d_id && o_c_id == c_id", {sele0, scan2}));    
        QueryTree::Operator* scan0 = tree.insert(new QueryTree::Operator(2, QueryTree::Type::tablescan, schema->findRelation("orderline")));
        QueryTree::Operator* join0 = tree.insert(new QueryTree::Operator(1, QueryTree::Type::join, {"o_w_id", "ol_w_id", "o_d_id", "ol_d_id", "o_id", "ol_o_id"},"o_w_id == ol_w_id && o_d_id == ol_d_id && o_id == ol_o_id", {join1, scan0})); 
        QueryTree::Operator* proj0 = tree.insert(new QueryTree::Operator(0, QueryTree::Type::print, {"c_first","c_last","o_all_local","ol_amount"}, {join0}));
       
        std::ofstream query_generated;
        query_generated.open ( tree.name+".hpp" );
        query_generated << tree.produce(proj0);
        query_generated.close();
        
#if 0
        std::cout <<  "-----------------------------" << std::endl;
        for (auto& op : tree.operators)
        {
            
            std::cout << TypeNames[(unsigned)op->type] << ": ";
            for (auto& a :  op->pushedAttributes)
                for (auto& b :  a)
                    std::cout << b.first.to_string() << "-" << b.second << ", ";
            std::cout << std::endl;
        }
        std::cout <<  "-----------------------------" << std::endl;
# endif
       
        
#if 0
        operatorHead head;
        tablescan scan(&head);
        head.child = &scan;
        scan.childs.at(0) = &schema->relations.at(0);
        head.produce();
# endif
        
    }
    catch ( ParserError& e )
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
 
}