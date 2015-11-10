#ifndef H_operator_hpp
#define H_operator_hpp

#include <iostream>
#include <array>
# include <memory>
#include <string>
#include <set>
#include <map>
#include <algorithm> 
#include "Schema.hpp"


# define UP tree->out << ident(identLvl)
# define UPP tree->out  << ident(identLvl+1)

 const char *TypeNames[4] = 
    {
        "tablescan",
        "print",
        "join", 
        "selection"
    };
    


struct FieldType
{

    Types::Tag type;
    unsigned len;
    unsigned len2;
    
    std::string to_string () const
    {

        switch ( type )
        {
        case Types::Tag::Integer:
            return "Integer";
        case Types::Tag::Numeric:
        {
            std::stringstream ss;
            ss << "Numeric<" << len << ", " << len2 << ">";
            return ss.str();
        }
        case Types::Tag::Varchar:
        {
            std::stringstream ss;
            ss << "Varchar<" << len << ">";
            return ss.str();
        }
        case Types::Tag::Timestamp:
        {
            std::stringstream ss;
            ss << "Timestamp";
            return ss.str();
        }
        case Types::Tag::Char:
        {
            std::stringstream ss;
            ss << "Char<" << len << ">";
            return ss.str();
        }
        }
        throw;
    }
};

inline bool operator <(const FieldType& lhs, const FieldType& rhs)
{
    return (unsigned)lhs.type < (unsigned)rhs.type;
}

inline bool operator == (const FieldType& lhs, const FieldType& rhs)
{
    return (unsigned)lhs.type == (unsigned)rhs.type;
}


std::string ident(unsigned identLvl)
{
    std::stringstream ss;
    for (unsigned i = 0;i < identLvl;i++)
        ss << '\t';
    return ss.str();
}

struct QueryTree
{
    using infUnit = std::pair<FieldType, std::string>;
    using informationUnitVector = std::vector<std::pair<FieldType, std::string>>;
    using equiExprTuple = std::vector<std::pair<std::pair<FieldType, std::string>, std::pair<FieldType, std::string>>>;
    
    static equiExprTuple condSideExpr(std::string condition, informationUnitVector &leftAttr, informationUnitVector &rightAttr)
    {
        equiExprTuple result;
        std::string delim = " && ";
        auto start = 0U;
        auto end = condition.find(delim);
        
        while (true)
        {
            auto str2 = condition.substr(start, end - start);
            
            std::string delim2 = " == ";
            std::pair<infUnit, bool> left;
            std::pair<infUnit, bool> right;
            left.second = false; right.second = false;
            std::vector<std::string> expr;
            auto start2 = 0U;
            auto end2 = str2.find(delim2);
            expr.push_back(str2.substr(start2, end2));
            if (end2 == std::string::npos)
                break;                                      ///< no equation
            start2 = end2 + delim2.length();
            end2 = str2.find(delim2, start2);
            expr.push_back(str2.substr(start2, end2));
            
            
            for (auto& k : leftAttr)
                if (k.second.compare(expr.back()) == 0)
                {
                    left = {{k.first, expr.back()},true};
                    expr.pop_back();
                    break;
                }
            for (auto& k : rightAttr)
                if (k.second.compare(expr.back()) == 0)
                {
                    right = {{k.first, expr.back()},true};
                    expr.pop_back();
                    break;
                } 
            if (expr.size())    
                for (auto& k : leftAttr)
                    if (k.second.compare(expr.back()) == 0)
                    {
                        left = {{k.first, expr.back()},true};
                        expr.pop_back();
                        break;
                    }
            if (!(left.second && right.second))
                throw;                                      ///< very bad, not suitable for equi/hash join
            result.push_back({left.first, right.first});
            if (end == std::string::npos)
                break;
            start = end + delim.length();
            end = condition.find(delim, start);      
        }
        return result;

    }
    
    
    enum class Type : unsigned
    {
        tablescan, print, join, selection
    };
    struct Operator
    {   
        QueryTree* tree;
        
        uint32_t operatorId;
        QueryTree::Type type;
        std::string condition;
        std::vector<std::string> requAttributes;
        /// for tablescan
        Schema::Relation* relation;
        /// Information Unit: type, name, requiredAs
        std::vector<informationUnitVector> neededAttributes; 
        std::vector<informationUnitVector> pushedAttributes; 
        
        std::vector<QueryTree::Operator*> childs;
        QueryTree::Operator* parent;
        
        Operator(uint64_t id, QueryTree::Type t, std::vector<std::string> req,  std::vector<QueryTree::Operator*> ch) : 
            operatorId(id), type(t), requAttributes(req), relation(nullptr), childs(ch), parent(nullptr)
        {
            for (auto&c : ch)
                c->parent = this;
        }
        Operator(uint64_t id, QueryTree::Type t, std::vector<std::string> req, std::string op, std::vector<QueryTree::Operator*> ch) : 
            operatorId(id), type(t), condition (op), requAttributes(req), relation(nullptr), childs(ch), parent(nullptr)
        {
            for (auto&c : ch)
                c->parent = this;
        }
        
        /* constructor for tablescan */
        Operator(uint64_t id, QueryTree::Type t, Schema::Relation* rel) : operatorId(id), type(t), parent(nullptr)
        {
            relation = rel;
        }
        
        void produce(unsigned identLvl)
        {
            switch ( type )
            {
                case QueryTree::Type::tablescan:
                {
                    UP << "for (unsigned var_" << relation->name << "_line = 0; var_" << relation->name << "_line < " << relation->name << "_column_relation.size(); var_" << relation->name << "_line++)\n";
                    UP << "{\n";
                    for (auto& attr : pushedAttributes.at(0))
                    {
                        UPP << attr.first.to_string() <<  "& " << attr.second << " = " << relation->name << "_column_relation." << attr.second << ".at(var_" << relation->name << "_line);\n";
                    }
                    parent->consume(this, identLvl+1);
                    UP << "}\n";
                    break;
                }
                case QueryTree::Type::selection:
                {
                    childs.at(0)->produce(identLvl);
                    break;
                }
                case QueryTree::Type::join:
                {
                    UP << "std::unordered_map<tuple<";
                    //condSideExpr(condition, neededAttributes.at(0), true);
                    auto buildAttributes = condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                    for (auto it=buildAttributes.begin(); it!=buildAttributes.end(); ++it)
                        tree->out << it->first.first.to_string() << (std::next(it) != buildAttributes.end() ? ", " : "");
                    tree->out << ">, tuple<";
                    for (auto it=pushedAttributes.at(0).begin(); it!=pushedAttributes.at(0).end(); ++it)
                        tree->out << it->first.to_string() << (std::next(it) != pushedAttributes.at(0).end() ? ", " : "");           
                    tree->out << ">> hashtable_op" << operatorId << ";\n";
                    childs.at(0)->produce(identLvl);
                    
                    childs.at(1)->produce(identLvl);
                    break;
                }
                case QueryTree::Type::print:
                {
                    childs.at(0)->produce(identLvl);
                    break;
                }
            }
            
        }
        
        void consume(QueryTree::Operator* child, unsigned identLvl)
        {
            switch ( type )
            {
                case QueryTree::Type::tablescan:
                {
                    break;
                }
                case QueryTree::Type::selection:
                {
                    UP << "if (";
                    tree->out << condition << " && ";
                    tree->out << "true)\n";
                    UP << "{\n";
                    parent->consume(this, identLvl+1);
                    UP << "}\n";
                    break;
                }
                case QueryTree::Type::join:
                {
                    /// left
                    if (child == childs.at(0))
                    {
                        UP << "hashtable_op" << operatorId << ".insert({std::make_tuple(";
                        auto buildAttributes =  condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                        for (auto it=buildAttributes.begin(); it!=buildAttributes.end(); ++it)
                            tree->out << it->first.second << (std::next(it) != buildAttributes.end() ? ", " : "");
                        tree->out << "), std::make_tuple(";
                        for (auto it=pushedAttributes.at(0).begin(); it!=pushedAttributes.at(0).end(); ++it)
                            tree->out << it->second << (std::next(it) != pushedAttributes.at(0).end() ? ", " : ""); 
                        tree->out << ")});\n";
                    /// right
                    }else{ 
                        UP << "auto key" << operatorId << " = std::make_tuple(";
                        auto probeAttributes = condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                        for (auto it=probeAttributes.begin(); it!=probeAttributes.end(); ++it)
                            tree->out << it->second.second << (std::next(it) != probeAttributes.end() ? ", " : "");
                        tree->out << ");\n";
                        UP << "if ( hashtable_op" << operatorId << ".count (key" << operatorId << ") > 0 )\n";
                        UP << "{\n";
                        UPP << "auto& value = hashtable_op" << operatorId << ".at(key" << operatorId << ");\n";
                        unsigned i = 0; 
                        for (auto& p :  pushedAttributes.at(0))
                        {
                            UPP << p.first.to_string() <<  "& " << p.second << " = " << "std::get<" << i << ">(value);\n";
                            i++;
                        }
                        for (auto& p :  pushedAttributes.at(1))
                        {
                            UPP << "/// " << p.first.to_string() <<  " " << p.second << ";\n";
                        }
                        parent->consume(this, identLvl+1);
                        UP << "}\n";
                        
                        
                    }
                    break;
                }
                case QueryTree::Type::print:
                {
                    UP << "out << ";
                    for (auto it=neededAttributes.at(0).begin(); it!=neededAttributes.at(0).end(); ++it)
                          tree->out  <<  it->second << (std::next(it) != neededAttributes.at(0).end() ? "<<'|'<<" : "<<");
                    tree->out <<  "std::endl;\n";
                    break;
                }
                throw;
            }
        }
        
        
        informationUnitVector 
        getAvailableAttributes(std::vector<std::string> requestedAttributes)
        {
            if (type ==  QueryTree::Type::tablescan)
            {
                pushedAttributes.push_back({});
                for (auto&attr  : relation->attributes )
                    
                    for (auto& r :  requestedAttributes)
                    {
                        if (attr.name.compare(r) == 0)
                        {
                            
                            pushedAttributes.at(0).push_back({{attr.type, attr.len, attr.len2}, attr.name});
                            continue;
                        }  
                    }                  
                    return pushedAttributes.at(0);
            }
            else
            {
                informationUnitVector confirmedAttributes;
                std::vector<std::string> requiredAll = requestedAttributes;
                for (auto& r : requAttributes)
                {
                    if (!(std::find(requiredAll.begin(), requiredAll.end(), r)!= requiredAll.end()))
                        requiredAll.push_back(r);
                }
                        
                for (auto& child :  childs)
                {
                    neededAttributes.push_back({});
                    pushedAttributes.push_back({});
                    informationUnitVector result = child->getAvailableAttributes(requiredAll);
                    for (auto& attrIt :  result)
                    {
                        
                        /// those are needed in upper operators
                        if (std::find(requestedAttributes.begin(), requestedAttributes.end(), attrIt.second)!= requestedAttributes.end())
                        {
                            pushedAttributes.back().push_back({attrIt.first, attrIt.second});
                            confirmedAttributes.push_back({attrIt.first, attrIt.second});
                        }
                        if (std::find(requAttributes.begin(), requAttributes.end(), attrIt.second)!= requAttributes.end())
                            neededAttributes.back().push_back({attrIt.first, attrIt.second});
                        
                    }
                }
                return confirmedAttributes;
            }
            
        }
        
        
        
    };

    std::string name;
    std::stringstream out;
    std::vector<QueryTree::Operator*> operators;
    
    QueryTree::Operator* insert(QueryTree::Operator* op)
    {
        operators.push_back(op);
        op->tree = this;
        return op;   
    }
     
    std::string produce(QueryTree::Operator* head)
    {
        head->getAvailableAttributes({});
        out << "std::string " << name << "()\n{\n";
        out << "\tstd::stringstream out;\n";
        head->produce(1);
        out << "\treturn out.str();\n";
        out << "}\n";
        return out.str();
    }
    
    
    QueryTree(std::string n) : name(n) {
       
    }
    
    ~QueryTree() {
        for (auto& ptr :  operators)
        {
            delete ptr;
        }
    }
    
    
    
    
};

# if 0


class _operatorChild {
    
public:
    virtual void produce() {}
    virtual ~_operatorChild() { } 
};


class _operatorParent : public _operatorChild{
    
public:
    virtual void consume(_operatorChild& op) {}
    
};

class operatorHead : public _operatorParent
{
public:
    
    
    
    _operatorChild* child;
    void consume(_operatorChild& op) {
        
    }
    
    operatorHead()
    {}
    operatorHead(decltype(operatorHead::child) & ch) : child(ch) 
    {}
    
    void produce() {
        child->produce();
    }
        
};

template <unsigned ARITY>
class _pushOperator : public _operatorParent
{
public:
    
    
    std::array<_operatorChild*, ARITY> childs;
    _operatorParent* parent;  

    _pushOperator(decltype(parent) p) : parent(p){}

};



class tablescan : public _pushOperator<1>
{
public: 
    
    
    tablescan(decltype(_pushOperator<1>::parent) p): _pushOperator<1>(p){}
    
    void produce()
    {
        tree->out << "for (auto& i: ";
        childs.at(0)->produce();
        tree->out << "_column.size())" <<  std::endl;
        tree->out << "{" << std::endl;
        parent->consume(*this);
        tree->out << "}" << std::endl;
        
        
    }
    
    void consume(_operatorChild& op)
    {
    //Do nothing   
    }
        
};

class join : public _pushOperator<2>
{
public: 
    
    join(decltype(_pushOperator<2>::parent) & p): _pushOperator<2>(p){}
    
    
    void produce()
    {
        tree->out << "build\n";
        
    }
    
    void consume(_operatorChild& op)
    {
    //Do nothing   
    }
        
};
#   endif

# endif