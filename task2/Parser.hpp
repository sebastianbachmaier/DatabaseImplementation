#ifndef H_Parser_hpp
#define H_Parser_hpp

#include <exception>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include "Schema.hpp"
#include "operator.hpp"


struct Parser {
   enum class State : unsigned { 
       Init, 
       Create, 
       Table, 
       Index,
       CreateTableBegin, 
       CreateTableEnd, 
       TableName, 
       
       Select,
       From, 
       FromMult, 
       Where, 

       
       Primary, 
       Key, 
       KeyListBegin,
       KeyName, 
       KeyListEnd, 
       
       IndexName,
       IndexOn, 
       IndexRelation, 
       IndexListBegin,
       IndexListEnd, 
       IndexKeyName,
       
       AttributeName, 
       AttributeTypeInt, 
       AttributeTypeTimestamp, 
       
       AttributeTypeChar, 
       CharBegin, 
       CharValue, 
       CharEnd, 
       
       AttributeTypeVarchar, 
       VarcharBegin, 
       VarcharValue, 
       VarcharEnd, 
       
       AttributeTypeNumeric, 
       NumericBegin, 
       NumericValue1, 
       NumericSeparator, 
       NumericValue2, 
       NumericEnd, 
       
       Not, 
       Null, 
       Separator, 
       Semicolon};
   std::string fileName;
   State state;
   Parser(const std::string& fileName) : fileName(fileName), state(State::Init), tree("schema"), opCount(0){}
   //~Parser() {};
   std::unique_ptr<Schema> parse();
   void parse(Schema& s,std::stringstream& in);
   QueryTree tree;
   unsigned opCount;

   private:
          
   void nextToken(unsigned line, const std::string& token, Schema& s);
};

#endif
