#include "Schema.hpp"

#include <sstream>
#include <map>
#include <set>


#define out2 out << '\t'
#define out3 out << '\t' << '\t'
#define pkeyt rel.name << "_primary_key_t"
#define ikeyt index.name << "_index_key_t"
#define colrelp rel.name << "_column_relation."
#define pkp rel.name << "_primary_key."
#define pk rel.name << "_primary_key"



static std::string type ( const Schema::Relation::Attribute& attr )
{
    Types::Tag type = attr.type;
    switch ( type )
    {
    case Types::Tag::Integer:
        return "Integer";
    case Types::Tag::Numeric:
    {
        std::stringstream ss;
        ss << "Numeric(" << attr.len << ", " << attr.len2 << ")";
        return ss.str();
    }
    case Types::Tag::Varchar:
    {
        std::stringstream ss;
        ss << "Varchar(" << attr.len << ")";
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
        ss << "Char(" << attr.len << ")";
        return ss.str();
    }
    }
    throw;
}

std::string Schema::toString() const
{
    std::stringstream out;

    for ( const Schema::Index& index : indexes )
    {
        out << "index " <<  index.name << " with ";
        const Schema::Relation& rel = relations[index.relation];
        for ( unsigned attrId : index.attributes )
            out  << rel.attributes[attrId].name << ( index.attributes.back() != attrId ? ", ": "" );
        out << " on " << rel.name << ";" <<  std::endl;
    }

    for ( const Schema::Relation& rel : relations )
    {
        out << rel.name << std::endl;
        out << "\tPrimary Key:";
        for ( unsigned keyId : rel.primaryKey )
            out << ' ' << rel.attributes[keyId].name;
        out << std::endl;
        for ( const auto& attr : rel.attributes )
            out << '\t' << attr.name << '\t' << type ( attr ) << ( attr.notNull ? " not null" : "" ) << std::endl;
    }
    return out.str();
}

static std::string cpptype ( const Schema::Relation::Attribute& attr )
{
    Types::Tag type = attr.type;
    switch ( type )
    {
    case Types::Tag::Integer:
        return "Integer";
    case Types::Tag::Numeric:
    {
        std::stringstream ss;
        ss << "Numeric<" << attr.len << ", " << attr.len2 << ">";
        return ss.str();
    }
    case Types::Tag::Varchar:
    {
        std::stringstream ss;
        ss << "Varchar<" << attr.len << ">";
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
        ss << "Char<" << attr.len << ">";
        return ss.str();
    }
    }
    throw;
}


std::string Schema::toCpp() const
{
    std::stringstream out;

    const std::set<std::string> str = {"neworder"};

    /*
     * headers
     */
    out << "#include <iostream>" << std::endl;
    out << "#include <vector>" << std::endl;
    out << "#include <string>" << std::endl;
    out << "#include <map>" << std::endl;
    out << "#include <unordered_map>" << std::endl;
    out << "#include <fstream>" << std::endl;
    out << "#include <sstream>" << std::endl;
    out << "#include <exception>" <<  std::endl;
    out << "#include \"hash.hpp\"" <<  std::endl;
    out << "#include \"Types.hpp\"" << std::endl;
    out << "#include <chrono>" << std::endl;


    out << "using namespace std::chrono;" << std::endl;
    out << "using namespace std;" << std::endl;

    /*
     * names
     */
    for ( const Schema::Relation& rel : relations )
    {
        out << "string " << rel.name << "_name = \"" << rel.name << "\";" <<  std::endl;
    }
    out <<  std::endl;




    /*
     * structs
     */

    for ( const Schema::Relation& rel : relations )
    {


        out << std::endl;
        out << "struct " << rel.name << "{" << std::endl;


        for ( const auto& attr : rel.attributes )
            out << '\t' << cpptype ( attr ) << ' ' << attr.name << ';' << std::endl;

        out << "};" << std::endl;
    }



    /*
     * row relations
     */
    for ( const Schema::Relation& rel : relations )
    {
        out << "std::vector<" << rel.name << "> " << rel.name << "_row_relation;" <<  std::endl;
    }


    /*
     *
     *
     *
     * COLUMNS
     *
     *
     *
     */


    /*
     * column relations
     */
    for ( const Schema::Relation& rel : relations )
    {


        out << std::endl;
        out << "struct " << rel.name << "_column{" << std::endl;


        for ( const auto& attr : rel.attributes )
            out2 << "std::vector<" << cpptype ( attr ) << "> " << attr.name << ';' << std::endl;
        
        if (rel.attributes.size()>0)
        {
            out2 << "uint64_t size() { return " << rel.attributes.at(0).name << ".size(); }\n";
            
        }
        out << "};" << std::endl;
        out << rel.name << "_column " <<  rel.name << "_column_relation;" <<  std::endl;

    }

    out <<  std::endl;



    /*
    * primary indexes
    */
    for ( const Schema::Relation& rel : relations )
    {


        if ( !rel.primaryKey.empty() )
        {
            std::string primaryKeyType;
            primaryKeyType += "std::tuple<";
            for ( unsigned keyId :  rel.primaryKey )
            {
                primaryKeyType += cpptype ( rel.attributes[keyId] );
                primaryKeyType += ( rel.primaryKey.back() != keyId ? ", ": ">" );
            }
            out << "using " << pkeyt << " = " << primaryKeyType << ";\n";
        }
    }
    for ( const Schema::Relation& rel : relations )
    {


        if ( !rel.primaryKey.empty() )
        {
            if ( str.count ( rel.name ) > 0 )
            {
                out <<  "std::map<" << pkeyt << ", uint64_t>";
            }
            else
            {
                out <<  "std::unordered_map<" << pkeyt << ", uint64_t>";

            }
            out << ' ' << rel.name << "_primary_key;" <<  std::endl;

        }


    }

    out <<  std::endl;

    /*
     * non primary indexes
     */
    for ( const Schema::Index& index : indexes )
    {
        const Schema::Relation& rel = relations[index.relation];
        std::string indexType;
        indexType += "std::tuple<";
        for ( unsigned attrId : index.attributes )
        {
            indexType +=  cpptype ( rel.attributes[attrId] );
            indexType += ( index.attributes.back() != attrId ? ", ": ">" );
        }
        out << "using " <<  ikeyt << " = " <<  indexType << ";\n";
    }
    for ( const Schema::Index& index : indexes )
    {

        const Schema::Relation& rel = relations[index.relation];
        if ( str.count ( rel.name ) > 0 )
        {
            out <<  "std::map<" << ikeyt << ", uint64_t>";
        }
        else
        {
            out <<  "std::unordered_map<" << ikeyt << ", uint64_t>";
        }

        out << ' ' << index.name << "_index;" <<  std::endl;
    }



    /*
     *
     * column insert
     *
     */


    for ( const Schema::Relation& rel : relations )
    {
        out << "bool " <<  rel.name << "_column_insert(" << rel.name << " element){" << std::endl;

        /*std::string primaryKeyType;
        primaryKeyType += "std::tuple<";
        for (unsigned keyId :  rel.primaryKey)
        {
            primaryKeyType += cpptype(rel.attributes[keyId]);
            primaryKeyType += (rel.primaryKey.back() != keyId ? ", ": ">");
        }*/

        // see if already exists
        if ( !rel.primaryKey.empty() )
        {
            out2 << "if (" << rel.name << "_primary_key.count(" << pkeyt << '(';
            for ( unsigned keyId :  rel.primaryKey )
                out<< "element." << rel.attributes[keyId].name << ( rel.primaryKey.back() != keyId ? ", ": ")) != 0)\n\t\treturn false;\n" );

        }
        // insert element in column
        for ( const auto& attr : rel.attributes )
            out2 << rel.name << "_column_relation." << attr.name << ".push_back(element." << attr.name << ");" << std::endl;

        if ( !rel.primaryKey.empty() )
        {
            out2 <<  rel.name << "_primary_key.insert(std::pair<" << pkeyt << ", uint64_t>(" << pkeyt << '(';
            for ( unsigned keyId :  rel.primaryKey )
                out<< "element." << rel.attributes[keyId].name << ( rel.primaryKey.back() != keyId ? ", ": ")," );
            out <<  rel.name << "_column_relation." << rel.attributes[rel.primaryKey[0]].name << ".size()-1));" <<  std::endl;
        }

        for ( const Schema::Index& index : indexes )
        {

            const Schema::Relation& rel_index = relations[index.relation];
            if ( &rel == &rel_index )
            {

                out2 <<   index.name << "_index.insert(std::pair<" << ikeyt << ", uint64_t>(" << ikeyt << "(";
                for ( unsigned attrId : index.attributes )
                    out << "element." << rel.attributes[attrId].name << ( index.attributes.back() != attrId ? ", ": "), " );
                out << colrelp <<  rel.attributes[index.attributes[0]].name << ".size()-1));\n";
            }
        }

        out2 << "return true;" << std::endl;
        out << "}" <<  std::endl;
    }


    for ( const Schema::Relation& rel : relations )
    {
        out << "bool " <<  rel.name << "_column_delete(uint64_t lineNr){" << std::endl;

        out2 << "if (lineNr >= " << colrelp << rel.attributes[0].name << ".size())\n\t\treturn false;\n";


        for ( const Schema::Index& index : indexes )
        {

            const Schema::Relation& rel_index = relations[index.relation];
            if ( &rel == &rel_index )
            {
                // There is an index on this, do something
                out2 << index.name << "_index.erase(" << ikeyt << "(";
                for ( unsigned attrId : index.attributes )
                    out << rel.name << "_column_relation." << rel.attributes[attrId].name << "[lineNr]" << ( index.attributes.back() != attrId ? ", ": "));\n" );

            }
        }

        if ( !rel.primaryKey.empty() )
        {
            out2 << pkp << "erase(" << pkeyt << "(";
            for ( unsigned keyId :  rel.primaryKey )
                out<< colrelp << rel.attributes[keyId].name << "[lineNr]" << ( rel.primaryKey.back() != keyId ? ", ": "));\n" );

        }

        out2 << "if (lineNr != " << colrelp << rel.attributes[0].name << ".size()-1){\n";
        for ( const auto& attr : rel.attributes )
            out3 <<  colrelp << attr.name << "[lineNr] = " << colrelp << attr.name << ".back()"<< ";\n";





        if ( !rel.primaryKey.empty() )
        {
            out3 << pk << "[" << pkeyt << "(";
            for ( const auto& keyId : rel.primaryKey )
                out << colrelp << rel.attributes[keyId].name << ".at(lineNr)"<< ( keyId != rel.primaryKey.back() ? ", ": ")] = lineNr;\n" );
        }

        /// update non primary index
        for ( const Schema::Index& index : indexes )
        {

            const Schema::Relation& rel_index = relations[index.relation];
            if ( &rel == &rel_index )
            {

                out3 <<   index.name << "_index[" << ikeyt << "(";
                for ( const auto& keyId : index.attributes )
                    out << colrelp << rel.attributes[keyId].name << ".at(lineNr)"<< ( keyId != index.attributes.back() ? ", ": ")] = lineNr;\n" );

            }
        }



        out2 << "}\n";
        for ( const auto& attr : rel.attributes )
            out2 << colrelp << attr.name << ".pop_back();\n";
        out2 << "return true;\n";

        out << "}\n";
    }


    /*
    * tbl-load
    */

    const char* TBL_PATH = "../tpc-c/tbl";

    for ( const Schema::Relation& rel : relations )
    {
        out << "static void load_" << rel.name << "()" << std::endl;
        out << "{" << std::endl;
        out << '\t' << "string line;" << std::endl;
        out << '\t' << "ifstream myfile;" << std::endl;
        out << '\t' << "myfile.open ( \"" << TBL_PATH << "/tpcc_" << rel.name << ".tbl\" );" << std::endl;
        out << '\t' << "if ( myfile.is_open() ) " << std::endl;
        out << '\t' << "{" << std::endl;
        out << '\t' << '\t' << "while ( getline ( myfile,line ) )" << std::endl;
        out << '\t' << '\t' << "{" << std::endl;
        out << '\t' << '\t' << '\t' << "istringstream ss ( line );" << std::endl;
        out << '\t' << '\t' << '\t' << "string token;" << std::endl;
        out << '\t' << '\t' << '\t' << rel.name << " element;" <<  std::endl;
        for ( const auto& attr : rel.attributes )
        {
            out << '\t' << '\t' << '\t' << "getline ( ss, token, '|' );" << std::endl;
            out << '\t' << '\t' << '\t' << "element." << attr.name << " = " << cpptype ( attr ) << "::castString ( token.c_str(), token.size() )" << ';' << std::endl;
        }

        out << '\t' << '\t' << '\t' << rel.name << "_column_insert(element);" << std::endl;
        out << '\t' << '\t' << "}" << std::endl;
        out << '\t' << '\t' << "myfile.close();" <<  std::endl;
        out << '\t' << "}" << std::endl;
        out << "}" << std::endl;
    }


    out << std::endl;

    out << "void init_tbl() {" << std::endl;
    out2 <<  " auto start=high_resolution_clock::now();\n";
    for ( const Schema::Relation& rel : relations )
    {
        out2 <<  "start=high_resolution_clock::now();\n";
        out2 << "load_" << rel.name << "();" << std::endl;
        out2 << "cout << \"Load: "<< rel.name <<": \" << duration_cast<duration<double>> ( high_resolution_clock::now()-start ).count() << \"s\" << endl;\n";

    }
    out << "}" <<  std::endl;

    return out.str();
}



// kate: indent-mode cstyle; indent-width 4; replace-tabs on; 
