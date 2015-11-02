#ifndef H_Schema_hpp
#define H_Schema_hpp

#include <vector>
#include <string>
#include "SchemaTypes.hpp"

struct Schema {
   struct Relation {
      struct Attribute {
         std::string name;
         Types::Tag type;
         unsigned len;
         unsigned len2;
         bool notNull;
         Attribute() : len(~0), notNull(true) {}
      };
      std::string name;
      std::vector<Schema::Relation::Attribute> attributes;
      std::vector<unsigned> primaryKey;
      Relation(const std::string& name) : name(name) {}
   };
   
   struct Index {
       std::string name;
       unsigned relation;
       std::vector<unsigned> attributes;
       Index(const std::string& name) : name(name) {}
   };
   std::vector<Schema::Index> indexes;
   std::vector<Schema::Relation> relations;
   std::string toString() const;
   
   std::string toCpp() const;
};
#endif
