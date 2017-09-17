///
/// \class db_map db_map.h
///
/// \brief Handles information about generalized data sources
///
/// This class provides users with an easy way to search and access a large
/// number of data sources, of various types, without need to know ahead of time
/// what the data sources will be, nor how the data will be used.
///
/// At its core, this class manages a database map that allows users to:
///
/// - identify a set of fields, from various database tables, that will be available to the program;
/// - attach user-defined <em>use codes</em> to those fields;
/// - create complex search queries for specific kinds of information; and
/// - retrieve the results.
///


#include <stdexcept>

#include "db_map.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Load source and field definitions from database
///
/// \param[in]  db         database connection, which must currently be open
/// \param[in]  src_defs   database table with the source definitions
/// \param[in]  fld_defs   database table with the field definitions
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void db_map::load_defs(database &db, std::string src_defs, std::string fld_defs)
{

    std::string query;
    std::string fields;

    std::vector< std::vector< std::string >> result_set;
    unsigned int num_rows;
    unsigned int num_cols;

    // Read the source definitions from the database.

    fields = "id, name, description, version, code, db_table, derived_from, writable";
    query  = "SELECT " + fields + " FROM " + src_defs;

    db.execute (query, result_set, num_rows, num_cols);

    // Set up the source definitions.

    unsigned int num_sources = num_rows;

    source_list.resize(num_sources);
    for (unsigned int i=0; i<num_sources; i++)
    {
        source_list[i].id           = result_set[i][0];
        source_list[i].name         = result_set[i][1];
        source_list[i].description  = result_set[i][2];
        source_list[i].version      = result_set[i][3];
        source_list[i].code         = result_set[i][4];
        source_list[i].db_table     = result_set[i][5];
        source_list[i].derived_from = result_set[i][6];

        if (result_set[i][7] == "yes")
            source_list[i].writable = true;
        else
            source_list[i].writable = false;
    }

    // Find any sources that were derived from another source (the parent), and add
    // them to the list of children for that parent.

    for (unsigned int i=0; i<num_sources; i++)
        if (source_list[i].derived_from.empty())
            source_list[i].my_parent = -1;
        else
        {
            source_list[i].my_parent = i;
            for (unsigned int j=0; j<num_sources; j++)
                if (source_list[i].derived_from == source_list[j].id)
                {
                    source_list[j].my_children.push_back(i);
                    break;
                }
        }

    // Read the field definitions from the database.

    fields = "id, z_sour, code, name, db_field, writable";
    query  = "SELECT " + fields + " FROM " + fld_defs;

    result_set.clear();

    db.execute (query, result_set, num_rows, num_cols);

    // Set up the source field definitions.

    unsigned int num_field_defs = num_rows;

    for (unsigned int i=0; i<num_field_defs; i++)
    {
        // Find the source that this field belongs to.

        int source_num = -1;
        for (unsigned int j=0; j<num_sources; j++)
            if (source_list[j].id == result_set[i][1])
            {
                source_num = j;
                break;
            }
        if (source_num < 0)
        {
            throw std::runtime_error("Field definition for a non-existent source");
        }

        // Save the information about the new field.

        field_def new_field;

        new_field.code     = result_set[i][2];
        new_field.name     = result_set[i][3];
        new_field.db_field = result_set[i][4];

        // Add the new field to the source.

        source_list[source_num].field_list.push_back(new_field);

    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of sources
///
////////////////////////////////////////////////////////////////////////////////////////////////////

int db_map::num_sources () const
{
    return source_list.size();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of field definitions for a source
///
/// \param[in]  source_num   Source number
///
/// \return     number of fields
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

int db_map::num_fields (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].field_list.size();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the descriptive name of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source name
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::src_name (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].name;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the long description of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source description
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::src_description (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].description;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the version of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source description
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::src_version (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].version;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the use code of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source description
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::src_code (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].code;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the database table name for a source
///
/// \param[in]  source_num   Source number
///
/// \return     database table
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::src_db_table (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].db_table;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Find the parent of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source number of the parent or -1 if there is no parent
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

int db_map::src_parent (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].my_parent;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Find the children of a source
///
/// \param[in]  source_num   Source number
///
/// \return     source children
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> db_map::src_children (int source_num) const
{
    test_input (source_num);
    return source_list[source_num].my_children;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the descriptive name of a field
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     field name
///
/// \exception std::out_of_range thrown if the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::fld_name (int source_num, int field_num) const
{
    test_inputs (source_num, field_num);
    return source_list[source_num].field_list[field_num].name;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the field code
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     field code
///
/// \exception std::out_of_range thrown if the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::fld_code (int source_num, int field_num) const
{
    test_inputs (source_num, field_num);
    return source_list[source_num].field_list[field_num].code;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get a database field name
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     database field name
///
/// \exception std::out_of_range thrown if the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_map::fld_db_name (int source_num, int field_num) const
{
    test_inputs (source_num, field_num);
    return source_list[source_num].field_list[field_num].db_field;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Make sure the source number is valid
///
/// \param[in]  source_num   Source number
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void db_map::test_input (int source_num) const
{
    if (source_num < 0 || source_num >= num_sources())
        throw std::out_of_range("Source number in db_map:: is out of range");
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Make sure the source and field numbers are valid
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void db_map::test_inputs (int source_num, int field_num) const
{
    if (source_num < 0 || source_num >= num_sources() ||
            (field_num < 0 || field_num >= num_fields(source_num)))
        throw std::out_of_range("Source or field number in db_map:: is out of range");
}
