#ifndef H_Schema_hpp
#define H_Schema_hpp


#include <vector>
#include <string>
#include "SchemaTypes.hpp"

class ParserError : std::exception {
   std::string msg;
   unsigned line;
   public:
   ParserError(unsigned line, const std::string& m) : msg(m), line(line) {}
   ~ParserError() throw() {}
   const char* what() const throw() {
      return msg.c_str();
   }
};

struct Schema
{
    struct Relation
    {
        struct Attribute
        {
            std::string name;
            Types::Tag type;
            unsigned len;
            unsigned len2;
            bool notNull;
            Attribute() : len ( ~0 ), notNull ( true ) {}
        };
        std::string name;
        std::vector<Schema::Relation::Attribute> attributes;
        std::vector<unsigned> primaryKey;
        Relation ( const std::string& name ) : name ( name ) {}
        
    };
    
    Schema::Relation* findRelation(std::string relName)
    {
        for (auto& rel :  relations)
        {
            if (relName.compare(rel.name) ==  0)
                return &rel;
        }
        throw ParserError(0, "'"+relName+"' is no relation");
    }
    std::pair<Schema::Relation*,Schema::Relation::Attribute*> relAttrLookup(std::string name)
    {
        for (auto& rel :  relations)
        {
            for (auto& attr :  rel.attributes)
            {
                if (attr.name.compare(name) == 0)
                    return std::make_pair(&rel,&attr);
            };
        }
        throw;
    }

    struct Index
    {
        std::string name;
        unsigned relation;
        std::vector<unsigned> attributes;
        Index ( const std::string& name ) : name ( name ) {}
    };

    std::vector<Schema::Index> indexes;
    std::vector<Schema::Relation> relations;
    std::string toString() const;

    std::string toHpp() const;
    std::string toCpp() const;
};
#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
