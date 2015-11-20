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


# define UP tree->out << identOp(identLvl)
# define UPP tree->out  << identOp(identLvl+1)
# define UPPP tree->out  << identOp(identLvl+2)


static const char *TypeNames[4] = 
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


static std::string identOp(unsigned identLvl)
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
        bool joinMergable;
        /// for tablescan
        std::vector<Schema::Relation*> relation;
        /// Information Unit: type, name, requiredAs
        std::vector<informationUnitVector> neededAttributes; 
        std::vector<informationUnitVector> pushedAttributes; 
        
        std::vector<QueryTree::Operator*> childs;
        QueryTree::Operator* parent;
        
        Operator(uint64_t id, QueryTree::Type t, std::vector<std::string> req,  std::vector<QueryTree::Operator*> ch) : 
            operatorId(id), type(t), requAttributes(req),joinMergable(true), childs(ch), parent(nullptr)
        {
            for (auto&c : ch)
                c->parent = this;
        }
        Operator(uint64_t id, QueryTree::Type t, std::vector<std::string> req, std::string op, std::vector<QueryTree::Operator*> ch) : 
            operatorId(id), type(t), condition (op), requAttributes(req),joinMergable(true), childs(ch), parent(nullptr)
        {
            for (auto&c : ch)
                c->parent = this;
        }
        
        void addChild(QueryTree::Operator* op)
        {
            childs.push_back(op);
            op->parent = this;
        }
        
        /* constructor for tablescan */
        Operator(uint64_t id, QueryTree::Type t, Schema::Relation* rel) : operatorId(id), type(t),joinMergable(true), parent(nullptr)
        {
            relation.push_back(rel);
        }
        
        void produce(unsigned identLvl)
        {
            switch ( type )
            {
                case QueryTree::Type::tablescan:
                {
                    UP << "for (unsigned var_" << relation.at(0)->name << "_line = 0; var_" << relation.at(0)->name << "_line < db->" << relation.at(0)->name << "_column_relation.size(); var_" << relation.at(0)->name << "_line++)\n";
                    UP << "{\n";
                    for (auto& attr : pushedAttributes.at(0))
                    {
                        UPP << attr.first.to_string() <<  "& " << attr.second << " = db->" << relation.at(0)->name << "_column_relation." << attr.second << ".at(var_" << relation.at(0)->name << "_line);\n";
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
                    UP << "std::unordered_multimap<tuple<bool";
                    //condSideExpr(condition, neededAttributes.at(0), true);
                    auto buildAttributes = condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                    for (auto it=buildAttributes.begin(); it!=buildAttributes.end(); ++it)
                        tree->out << ',' <<  it->first.first.to_string();
                        // << (std::next(it) != buildAttributes.end() ? ", " : "");
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
                        UP << "hashtable_op" << operatorId << ".insert({std::make_tuple(true";
                        auto buildAttributes =  condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                        for (auto it=buildAttributes.begin(); it!=buildAttributes.end(); ++it)
                            tree->out << ',' << it->first.second;
                            // << (std::next(it) != buildAttributes.end() ? ", " : "");                            
                        tree->out << "), std::make_tuple(";
                        for (auto it=pushedAttributes.at(0).begin(); it!=pushedAttributes.at(0).end(); ++it)
                            tree->out << it->second << (std::next(it) != pushedAttributes.at(0).end() ? ", " : ""); 
                        tree->out << ")});\n";
                    /// right
                    }else{ 
                        UP << "auto key" << operatorId << " = std::make_tuple(true";
                        auto probeAttributes = condSideExpr(condition, neededAttributes.at(0), neededAttributes.at(1));
                        for (auto it=probeAttributes.begin(); it!=probeAttributes.end(); ++it)
                            tree->out << ',' << it->second.second;
                              //<< (std::next(it) != probeAttributes.end() ? ", " : "");
                        tree->out << ");\n";
                        UP << "if ( hashtable_op" << operatorId << ".count (key" << operatorId << ") > 0 )\n";
                        UP << "{\n";
                        UPP << "auto range = hashtable_op" << operatorId << ".equal_range(key" << operatorId << ");\n";
                        UPP << "for (auto it = range.first; it != range.second; ++it) {\n";
                        unsigned i = 0; 
                        for (auto& p :  pushedAttributes.at(0))
                        {
                            UPPP << p.first.to_string() <<  "& " << p.second << " = " << "std::get<" << i << ">(it->second);\n";
                            i++;
                        }
                        for (auto& p :  pushedAttributes.at(1))
                        {
                            UPPP << "/// " << p.first.to_string() <<  " " << p.second << ";\n";
                        }
                        parent->consume(this, identLvl+2);
                        UP << "\t}\n}\n";
                        
                        
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
        
        
        void cleanAttributes()
        {
            for (auto& op : childs)
            {
                op->cleanAttributes();   
            }
            this->neededAttributes.clear();
            this->pushedAttributes.clear();
        }
        
        informationUnitVector 
        getAvailableAttributes(std::vector<std::string> requestedAttributes)
        {
            if (type ==  QueryTree::Type::tablescan)
            {
                pushedAttributes.push_back({});
                for (auto&attr  : relation.at(0)->attributes )
                    
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
                unsigned attrsize = 0;
                for (auto& vec :  neededAttributes)
                    attrsize += vec.size();
                    
                if (attrsize != requAttributes.size())
                {
                    for (auto& s_attr :  requAttributes)
                    {
                        bool found = false;
                        for (auto& v_attr : neededAttributes)
                        {
                            for (auto& attr : v_attr)
                            {
                                if (attr.second == s_attr)
                                {
                                    found = true;
                                }
                            }  
                        }
                        if (!found)
                            throw ParserError(0, "Unknown Attribute '"+s_attr+"'");
                    }
                }
                return confirmedAttributes;
            }
            
        }  
    };
    
    static std::vector<Schema::Relation*>& allocateRelations(QueryTree::Operator* thisOp)
    {
        for (QueryTree::Operator* op : thisOp->childs)
        {
            std::vector<Schema::Relation*>& tmp = allocateRelations(op);
            for (Schema::Relation* rel : tmp)
            {
                thisOp->relation.push_back(rel);
            }   
        }
        return thisOp->relation;
    }
    
    template <typename T>
    static bool IsSubset(std::vector<T> A, std::vector<T> B)
    {
        std::sort(A.begin(), A.end());
        std::sort(B.begin(), B.end());
        return std::includes(A.begin(), A.end(), B.begin(), B.end());
    }
    

    
    static bool predPushdownSelc(QueryTree::Operator* thisOp,QueryTree::Operator* sel, Schema& schema, std::vector<Schema::Relation*> needed)
    {
        // merge selecttions -> dont do this
        /*if (thisOp != sel && thisOp->type == QueryTree::Type::selection && thisOp->joinMergable == sel->joinMergable && std::is_permutation(thisOp->relation.begin(), thisOp->relation.end(), needed.begin()))
        {
            
        print(thisOp->tree->operators.front());
            // first delete selection from where it is
            // parent 
            for (unsigned i = 0; i  < sel->parent->childs.size(); i++)
            {
                if (sel->parent->childs.at(i) == sel)
                    sel->parent->childs.at(i)  = sel->childs.at(0);
            }
            
            // child
            for (auto& ch :  sel->childs)
                ch->parent = sel->parent;
                
            if (thisOp->condition.compare("") != 0)
            {
                thisOp->condition += " && "+sel->condition;
            }
            else{
                thisOp->condition += sel->condition;
            }
    
            for (auto& attr : sel->requAttributes)
            {
                thisOp->requAttributes.push_back(attr);
            }
            
            return true;
            
        }*/
        for (unsigned j = 0; j < thisOp->childs.size(); j++)
        {
            
            if (IsSubset(thisOp->childs.at(j)->relation, needed))
            {
                if (thisOp->childs.at(j)->type == QueryTree::Type::tablescan && thisOp != sel )
                {
                    
                    if (needed.size() == 1 && needed.at(0) == thisOp->childs.at(j)->relation.at(0))
                    {
                        std::cout << "insert " << sel->condition <<  " below " << TypeNames[(unsigned)thisOp->type] << " and " << TypeNames[(unsigned)thisOp->childs.at(j)->type] << std::endl;
                        print(thisOp->tree->operators.front());
                        // best to put selection right above tablescan
                        // first delete selection from where it is
                        // parent
                        for (unsigned i = 0; i  < sel->parent->childs.size(); i++)
                        {
                            if (sel->parent->childs.at(i) == sel)
                            {
                                sel->parent->childs.at(i)  = sel->childs.at(0);
                            }
                        }
                        // child
                        for (auto& ch :  sel->childs)
                            ch->parent = sel->parent;
                        //place child in    
                        thisOp->childs.at(j)->parent = sel;
                        sel->childs.at(0) = thisOp->childs.at(j);
                        thisOp->childs.at(j) = sel;
                        sel->parent = thisOp;
                        return true;
                    }
                } else if (predPushdownSelc(thisOp->childs.at(j), sel, schema, needed))
                {
                    return true;
                }
            }
        }
        // not further pushable, insert in join here?
        if (thisOp->type == QueryTree::Type::join && sel->joinMergable)
        {
            
        print(thisOp->tree->operators.front());
           // best to put selection right above tablescan
           // first delete selection from where it is
           // parent
            for (unsigned i = 0; i  < sel->parent->childs.size(); i++)
            {
                if (sel->parent->childs.at(i) == sel)
                {
                    sel->parent->childs.at(i)  = sel->childs.at(0); 
                }
            }
            // child
            for (auto& ch :  sel->childs)
                ch->parent = sel->parent;
            // push condition to join
            
            if (thisOp->condition.compare("") != 0)
            {
                thisOp->condition += " && "+sel->condition;
            }
            else{
                thisOp->condition += sel->condition;
            }
    
            for (auto& attr : sel->requAttributes)
            {
                thisOp->requAttributes.push_back(attr);
            }
                
            return true;
        }else{
            return false;
        }
    }
    
    static void predPushdown(QueryTree::Operator* thisOp, Schema& schema)
    {
        print(thisOp->tree->operators.front());
        bool rerun = true;
        while (rerun)
        {
            rerun = false;
            for (QueryTree::Operator* op : thisOp->childs)
            {
                if (op->type == QueryTree::Type::selection)
                {
                    // check if it's a selection to tablescan row
                    QueryTree::Operator* tmp = op;
                    while (tmp->type == QueryTree::Type::selection)
                        tmp = tmp->childs.at(0);
                    if (tmp->type == QueryTree::Type::tablescan && op != tmp)
                        continue;
                    std::vector<Schema::Relation*> rels;
                    for (std::string name : op->requAttributes)
                        rels.push_back(schema.relAttrLookup(name).first);
                    if (predPushdownSelc(op,op, schema, rels))
                    {
                        // children may have changes
                        rerun = true;
                        break;
                    }
                }
                predPushdown(op, schema);
            }
        }
    }
    
# define DEBUG 1
    
    static void optimizeQuery(QueryTree::Operator* head, Schema& schema)
    {      
        print(head);  
        std::cout << "RelationAllocate" << std::endl;
        allocateRelations(head);
        print(head);
        std::cout << "Predicate Pushdown" << std::endl;
        predPushdown(head, schema);
        print(head);
    }

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
        
        if (head->neededAttributes.empty())
            head->getAvailableAttributes({});
        out << "#include \"query_additions.hpp\"\n";
        out << "extern \"C\" std::string " << "query" << "(DATABASE* db)\n{\n";
        out << "\tstd::stringstream out;\n";
        head->produce(1);
        out << "\treturn out.str();\n";
        out << "}\n";
        return out.str();
    }
    
    
    QueryTree(std::string n) : name(n) {
       
    }
    
    static void print(QueryTree::Operator* head)
    {
# if DEBUG
      std::cout << std::endl;
      std::vector<QueryTree::Operator*> current {head};
      while(!current.empty())
      {
        std::vector<QueryTree::Operator*> tmp;
        for (auto op :  current)
        {
            for (auto ch : op->childs)
                tmp.push_back(ch);
            std::string needs;
            std::string rels;
            for (auto s : op->requAttributes)
                needs += s+",";            
            for (auto s : op->relation)
                rels += s->name+",";
            std::cout << TypeNames[(int)op->type] << '(' << (op->relation.size()>0 ? rels : "inherit") << '|'<< needs << '|' << op->condition << ")\t";
        }
        std::cout << std::endl;
        current = tmp;
        
      }
      std::cout <<  std::endl;
#   endif
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