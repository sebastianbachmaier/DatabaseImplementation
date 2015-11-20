#include "Parser.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>

namespace keyword {
   const std::string Primary = "primary";
   const std::string Key = "key";
   const std::string Create = "create";
   const std::string Select = "select";
   const std::string Where = "where";
   const std::string From = "from";
   const std::string And = "and";
   const std::string Table = "table";
   const std::string Index = "index";
   const std::string On = "on";
   const std::string Integer = "integer";
   const std::string Numeric = "numeric";
   const std::string Varchar = "varchar";
   const std::string Timestamp = "timestamp";
   const std::string Not = "not";
   const std::string Null = "null";
   const std::string Char = "char";
}

namespace literal {
   const char ParenthesisLeft = '(';
   const char ParenthesisRight = ')';
   const char Comma = ',';
   const char Star = '*';
   const char Semicolon = ';';
   const char Equal = '=';
   const char Apostroph = '\'';
}

std::unique_ptr<Schema> Parser::parse() {
    
    std::unique_ptr<Schema> s(new Schema());
    std::ifstream in;
    
    in.open(fileName);
    if (!in.is_open())
      throw ParserError(1, "cannot open file '"+fileName+"'");
    std::stringstream sstream;
    sstream << in.rdbuf();
    parse(*s, sstream);
    return std::move(s);
}


void Parser::parse(Schema& s, std::stringstream& in) {
   std::string token;
   unsigned line=1;
   while (in >> token) {
      std::string::size_type pos;
      std::string::size_type prevPos = 0;

      while ((pos = token.find_first_of(",)(;", prevPos)) != std::string::npos) {
         nextToken(line, token.substr(prevPos, pos-prevPos), s);
         nextToken(line, token.substr(pos,1), s);
         prevPos=pos+1;
      }
      nextToken(line, token.substr(prevPos), s);
      if (token.find("\n")!=std::string::npos)
         ++line;
   }
}

static bool isIdentifier(const std::string& str) {
   if (
      str==keyword::Primary ||
      str==keyword::Key ||
      str==keyword::Table ||
      str==keyword::Create ||
      str==keyword::Select ||
      str==keyword::From ||
      str==keyword::Where ||
      str==keyword::And ||
      str==keyword::On ||
      str==keyword::Index ||
      str==keyword::Integer ||
      str==keyword::Varchar ||
      str==keyword::Numeric ||
      str==keyword::Timestamp ||
      str==keyword::Not ||
      str==keyword::Null ||
      str==keyword::Char
   )
      return false;
   return str.find_first_not_of("'abcdefghijklmnopqrstuvwxyz_1234567890") == std::string::npos;
}

static bool isInt(const std::string& str) {
   return str.find_first_not_of("0123456789") == std::string::npos;
}

void Parser::nextToken(unsigned line, const std::string& token, Schema& schema) {
	if (getenv("DEBUG"))
		std::cerr << line << ": " << token << std::endl;
   if (token.empty())
      return;
   std::string tok;
   std::transform(token.begin(), token.end(), std::back_inserter(tok), tolower);
   switch(state) {
      case State::Semicolon: /* fallthrough */
      case State::Init:
         if (tok==keyword::Create)
            state=State::Create;
         else if (tok==keyword::Select)
         {
            state=State::Select;
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::print, {}, {})); 
            opCount++;
         }
         else
            throw ParserError(line, "Expected 'CREATE' or 'Select', found '"+token+"'");
         break;
     
      case State::Select:
        if (tok==keyword::From)
            state=State::From;
        else if (tok.size()==1 && tok[0]==literal::Star)
        {
            
        }
        else if (tok.size()==1 && tok[0]==literal::Comma)
        {
            
        }
        else if (isIdentifier(tok))
        {
            tree.operators.back()->requAttributes.push_back(token);
        }
        else 
            throw ParserError(line, "Expected Attribute, found '"+token+"'");
          break;
      case State::Create:
         if (tok==keyword::Table)
            state=State::Table;
         else if (tok==keyword::Index)
            state=State::Index;
         else
            throw ParserError(line, "Expected 'TABLE', found '"+token+"'");
         break;
     
     /*
      * 
      * Table
      * 
      */
      case State::Table:
         if (isIdentifier(tok)) {
            state=State::TableName;
            schema.relations.push_back(Schema::Relation(token));
         } else {
            throw ParserError(line, "Expected TableName, found '"+token+"'");
         }
         break;
      case State::TableName:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::CreateTableBegin;
         else
            throw ParserError(line, "Expected '(', found '"+token+"'");
         break;
     case State::From:
         if (isIdentifier(tok)) {
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::tablescan, schema.findRelation(token)));
            opCount++;
            state = State::FromMult;
         }else if (tok.size()==1 && tok[0] == literal::Comma)
             break;
          else if (tok==keyword::Where)
            throw ParserError(line, "Expected TableName, found '"+token+"'");
          else
            throw ParserError(line, "Expected 'From', found '"+token+"'");
         break;
      case State::FromMult:
         if (isIdentifier(tok)) {    
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::tablescan, schema.findRelation(token)));
            opCount++;
            // first do cross product
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::join, {}, "", {tree.operators.end()[-2], tree.operators.back()}));    
            opCount++;
         } else if (tok.size()==1 && tok[0] == literal::Comma)
             break;
         else if (tok.size()==1 && tok[0]==literal::Semicolon)
         {
            QueryTree::Operator* head = tree.operators.front();
            head->addChild(tree.operators.back());   
         }
           else if (tok==keyword::Where) {
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::selection, {}, "", {tree.operators.back()}));    
            opCount++;
            state = State::Where;
           }
          else
            throw ParserError(line, "Expected 'Where' or TableName, found '"+token+"'");
         break;
      case State::Where:
         if (isIdentifier(tok)) { 
            if (tok.size()>=1 && tok[0] == literal::Apostroph)
            {
                // string
                std::string txt = token;
                txt.front() = '"';
                txt.back() = '"';
                tree.operators.back()->condition += txt;
                tree.operators.back()->joinMergable = false;
            }else if (isInt(tok)){
                // int
                tree.operators.back()->condition += token;
                tree.operators.back()->joinMergable = false;
            }else{
                // var
                tree.operators.back()->requAttributes.push_back(token);
                tree.operators.back()->condition += token;
            }
         } else if  (tok==keyword::And) {
            tree.insert(new QueryTree::Operator(opCount, QueryTree::Type::selection, {}, "", {tree.operators.back()}));    
            opCount++;
         } else if (tok.size()==1 && tok[0] == literal::Equal){
            tree.operators.back()->condition += " == ";
         } else if (tok.size()==1 && tok[0] == literal::Semicolon) {
            QueryTree::Operator* head = tree.operators.front();
            head->addChild(tree.operators.back());
         } else {
            throw ParserError(line, "Expected 'Where', found '"+token+"'");
         }
         break;
     /*
      * 
      * Index
      * 
      */
     
     case State::Index:
         if (isIdentifier(tok)) {
            state=State::IndexName;
            schema.indexes.push_back(Schema::Index(token));
         } else {
            throw ParserError(line, "Expected IndexName, found '"+token+"'");
         }
         break;
     case State::IndexName:
         if (tok==keyword::On)
            state=State::IndexOn;
         else
            throw ParserError(line, "Expected , found '"+token+"'");
         break;
     case State::IndexOn:
         if (isIdentifier(tok)) {
            struct RelationNamePredicate {
               const std::string& name;
               RelationNamePredicate(const std::string& name) : name(name) {}
               bool operator()(const Schema::Relation& rel) const {
                  return rel.name == name;
               }
            };
            
            RelationNamePredicate p(token);
            auto it = std::find_if(schema.relations.begin(), schema.relations.end(), p);
            if (it == schema.relations.end())
               throw ParserError(line, "'"+token+"' is not a relation");
            
            schema.indexes.back().relation = std::distance(schema.relations.begin(), it);
            state=State::IndexRelation;
         } else {
            throw ParserError(line, "Expected RelationName, found '"+token+"'");
         }
         break;
     
     case State::IndexRelation:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::IndexListBegin;
         else
            throw ParserError(line, "Expected '(', found '"+token+"'");
         break;
     
     case State::IndexListBegin:
         if (isIdentifier(tok)) {
            struct IndexAttributeNamePredicate {
               const std::string& name;
               IndexAttributeNamePredicate(const std::string& name) : name(name) {}
               bool operator()(const Schema::Relation::Attribute& attr) const {
                  return attr.name == name;
               }
            };
            const auto& attributes = schema.relations[schema.indexes.back().relation].attributes;
            IndexAttributeNamePredicate p(token);
            auto it = std::find_if(attributes.begin(), attributes.end(), p);
            if (it == attributes.end())
               throw ParserError(line, "'"+token+"' is not an attribute of '"+schema.relations[schema.indexes.back().relation].name+"'");
            schema.indexes.back().attributes.push_back(std::distance(attributes.begin(), it));
            state=State::IndexKeyName;
         } else {
            throw ParserError(line, "Expected key attribute, found '"+token+"'");
         }
         break;
      case State::IndexKeyName:
         if (tok.size()==1 && tok[0] == literal::Comma)
            state=State::IndexListBegin;
         else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
            state=State::IndexListEnd;
         else
            throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
         break;
      case State::IndexListEnd:
         if (tok.size()==1 && tok[0]==literal::Semicolon)
            state=State::Semicolon;
         else
            throw ParserError(line, "Expected ';', found '"+token+"'");
         break;
     
     
      case State::Separator: /* fallthrough */
          
      /*
       * 
       * choose wether primary key or new field
       * 
       */
          
      case State::CreateTableBegin:
         if (tok.size()==1 && tok[0]==literal::ParenthesisRight) {
            state=State::CreateTableEnd;
         } else if (tok==keyword::Primary) {
            state=State::Primary;
         } else if (isIdentifier(tok)) {
            schema.relations.back().attributes.push_back(Schema::Relation::Attribute());
            schema.relations.back().attributes.back().name = token;
            state=State::AttributeName;
         } else {
            throw ParserError(line, "Expected attribute definition, primary key definition or ')', found '"+token+"'");
         }
         break;
     
     
      case State::CreateTableEnd:
         if (tok.size()==1 && tok[0]==literal::Semicolon)
            state=State::Semicolon;
         else
            throw ParserError(line, "Expected ';', found '"+token+"'");
         break;
      case State::Primary:
         if (tok==keyword::Key)
            state=State::Key;
         else
            throw ParserError(line, "Expected 'KEY', found '"+token+"'");
         break;
      case State::Key:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::KeyListBegin;
         else
            throw ParserError(line, "Expected list of key attributes, found '"+token+"'");
         break;
      case State::KeyListBegin:
         if (isIdentifier(tok)) {
            struct AttributeNamePredicate {
               const std::string& name;
               AttributeNamePredicate(const std::string& name) : name(name) {}
               bool operator()(const Schema::Relation::Attribute& attr) const {
                  return attr.name == name;
               }
            };
            const auto& attributes = schema.relations.back().attributes;
            AttributeNamePredicate p(token);
            auto it = std::find_if(attributes.begin(), attributes.end(), p);
            if (it == attributes.end())
               throw ParserError(line, "'"+token+"' is not an attribute of '"+schema.relations.back().name+"'");
            schema.relations.back().primaryKey.push_back(std::distance(attributes.begin(), it));
            state=State::KeyName;
         } else {
            throw ParserError(line, "Expected key attribute, found '"+token+"'");
         }
         break;
      case State::KeyName:
         if (tok.size()==1 && tok[0] == literal::Comma)
            state=State::KeyListBegin;
         else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
            state=State::KeyListEnd;
         else
            throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
         break;
      case State::KeyListEnd:
         if (tok.size()==1 && tok[0] == literal::Comma)
            state=State::Separator;
         else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
            state=State::CreateTableEnd;
         else
            throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
         break;
     
     /*
      * 
      * select attribute name
      * 
      */
     
     
      case State::AttributeName:
         if (tok==keyword::Integer) {
            schema.relations.back().attributes.back().type=Types::Tag::Integer;
            state=State::AttributeTypeInt;
         } else if (tok==keyword::Timestamp) {
            schema.relations.back().attributes.back().type=Types::Tag::Timestamp;
            state=State::AttributeTypeTimestamp;
         } else if (tok==keyword::Char) {
            schema.relations.back().attributes.back().type=Types::Tag::Char;
            state=State::AttributeTypeChar;
         } else if (tok==keyword::Numeric) {
            schema.relations.back().attributes.back().type=Types::Tag::Numeric;
            state=State::AttributeTypeNumeric;
         } else if (tok==keyword::Varchar) {
            schema.relations.back().attributes.back().type=Types::Tag::Varchar;
            state=State::AttributeTypeVarchar;
         }
         else throw ParserError(line, "Expected type after attribute name, found '"+token+"'");
         break;
     
     /*
      * 
      * Char
      * 
      */
     
     
     
      case State::AttributeTypeChar:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::CharBegin;
         else
            throw ParserError(line, "Expected '(' after 'CHAR', found'"+token+"'");
         break;
      case State::CharBegin:
         if (isInt(tok)) {
            schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
            state=State::CharValue;
         } else {
            throw ParserError(line, "Expected integer after 'CHAR(', found'"+token+"'");
         }
         break;
      case State::CharValue:
         if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
            state=State::CharEnd;
         else
            throw ParserError(line, "Expected ')' after length of CHAR, found'"+token+"'");
         break;
     
      /*
      * 
      * Varchar
      * 
      */
     
     
     
      case State::AttributeTypeVarchar:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::VarcharBegin;
         else
            throw ParserError(line, "Expected '(' after 'CHAR', found'"+token+"'");
         break;
      case State::VarcharBegin:
         if (isInt(tok)) {
            schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
            state=State::VarcharValue;
         } else {
            throw ParserError(line, "Expected integer after 'CHAR(', found'"+token+"'");
         }
         break;
      case State::VarcharValue:
         if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
            state=State::VarcharEnd;
         else
            throw ParserError(line, "Expected ')' after length of CHAR, found'"+token+"'");
         break;
     
     
     
     /*
      * 
      * Numeric
      * 
      */
     
     
      case State::AttributeTypeNumeric:
         if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
            state=State::NumericBegin;
         else
            throw ParserError(line, "Expected '(' after 'NUMERIC', found'"+token+"'");
         break;
      case State::NumericBegin:
         if (isInt(tok)) {
            schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
            state=State::NumericValue1;
         } else {
            throw ParserError(line, "Expected integer after 'NUMERIC(', found'"+token+"'");
         }
         break;
      case State::NumericValue1:
         if (tok.size()==1 && tok[0]==literal::Comma)
            state=State::NumericSeparator;
         else
            throw ParserError(line, "Expected ',' after first length of NUMERIC, found'"+token+"'");
         break;
      case State::NumericValue2:
         if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
            state=State::NumericEnd;
         else
            throw ParserError(line, "Expected ')' after second length of NUMERIC, found'"+token+"'");
         break;
      case State::NumericSeparator:
         if (isInt(tok)) {
            schema.relations.back().attributes.back().len2=std::atoi(tok.c_str());
            state=State::NumericValue2;
         } else {
            throw ParserError(line, "Expected second length for NUMERIC type, found'"+token+"'");
         }
         break;
     
      case State::CharEnd: /* fallthrough */
      case State::NumericEnd: /* fallthrough */
      case State::VarcharEnd: /* fallthrough */    
          
          
      /*
       * 
       * Integer
       * 
       */    
          
          
      case State::AttributeTypeInt:
         if (tok.size()==1 && tok[0]==literal::Comma)
            state=State::Separator;
         else if (tok==keyword::Not)
            state=State::Not;
         else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
				state=State::CreateTableEnd;
         else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
         break;
     
     /*
      * 
      * Timestamp
      * 
      */
     
    
    case State::AttributeTypeTimestamp:
         if (tok.size()==1 && tok[0]==literal::Comma)
            state=State::Separator;
         else if (tok==keyword::Not)
            state=State::Not;
         else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
                state=State::CreateTableEnd;
         else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
         break;
     
     
     /*
      * 
      * Not and Null
      * 
      */
     
     
      case State::Not:
         if (tok==keyword::Null) {
            schema.relations.back().attributes.back().notNull=true;
            state=State::Null;
         }
         else throw ParserError(line, "Expected 'NULL' after 'NOT' name, found '"+token+"'");
         break;
      case State::Null:
         if (tok.size()==1 && tok[0]==literal::Comma)
            state=State::Separator;
         else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
            state=State::CreateTableEnd;
         else throw ParserError(line, "Expected ',' or ')' after attribute definition, found '"+token+"'");
         break;
      default:
         throw;
   }
}
