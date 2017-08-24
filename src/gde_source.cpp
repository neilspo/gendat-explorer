///
/// \class gendat_source_list gde_source.h
///
/// \brief Handles information about GenDat sources
///
/// This class obtains definition information about GenDat sources from the database.
/// Database table `z_sour` holds general descriptive information about the sources, while
/// table `z_sour_field` holds further information about the fields that will be used
/// in any database table that is linked to a source.
///


#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <stdexcept>

#include "gde_source.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Load the source definitions from database
///
/// \param[in]  db   database connection, which must currently be open
///
/// \return     `true` if the source definitions were successfully loaded, `false` otherwise
///
/// \note
/// Any errors that are encountered will be reported with the `wxLogMessage` function.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool gendat_source_list::load_defs(database &db)
{

    std::string query;
    std::string fields;

    std::vector< std::vector< std::string >> result_set;
    unsigned int num_rows;
    unsigned int num_cols;

    // Try to read the GenDat source definitions from the database.
    // If this fails, then report the error and return false.

    fields = "id, name, description, version, db_table, repository, derived_from, writable, type";
    query  = "SELECT " + fields + " FROM z_sour";

    try
    {
        db.execute (query, result_set, num_rows, num_cols);
    }
    catch (std::runtime_error& exception)
    {
        wxLogMessage(exception.what());
        return false;
    }

    // Set up the GenDat source definitions.

    source_list.resize(num_rows);
    for (unsigned int i=0; i<num_rows; i++)
    {
        source_list[i].id           = result_set[i][0];
        source_list[i].name         = result_set[i][1];
        source_list[i].description  = result_set[i][2];
        source_list[i].version      = result_set[i][3];
        source_list[i].db_table     = result_set[i][4];
        source_list[i].repository   = result_set[i][5];
        source_list[i].derived_from = result_set[i][6];

        if (result_set[i][7] == "yes")
            source_list[i].writable = true;
        else
            source_list[i].writable = false;

        if (result_set[i][8] == "BIRT") source_list[i].type = BIRT;
        if (result_set[i][8] == "DEAT") source_list[i].type = DEAT;
        if (result_set[i][8] == "MARR") source_list[i].type = MARR;
    }

    // Find any GenDat sources that were derived from another source (the parent), and add
    // them to the list of children for that parent.

    for (unsigned int i=0; i<num_rows; i++)
        if (source_list[i].derived_from.empty())
            source_list[i].my_parent = -1;
        else
        {
            source_list[i].my_parent = i;
            for (unsigned int j=0; j<num_rows; j++)
                if (source_list[i].derived_from == source_list[j].id)
                {
                    source_list[j].my_children.push_back(i);
                    break;
                }
        }

    // Try to read the GenDat source field definitions from the database.
    // If this fails, then report the error and return false.

    fields = "id, z_sour, name, description, db_field, writable";
    query  = "SELECT " + fields + " FROM z_sour_field";

    result_set.clear();
    try
    {
        db.execute (query, result_set, num_rows, num_cols);
    }
    catch (std::runtime_error& exception)
    {
        wxLogMessage(exception.what());
        return false;
    }

    // Set up the GenDat source field definitions.

     unsigned int num_sources    = source_list.size();
     unsigned int num_field_defs = num_rows;

    for (unsigned int i=0; i<num_field_defs; i++)
    {
        // Find the GenDat source that this field belongs to.

        int source_num = -1;
        for (unsigned int j=0; j<num_sources; j++)
            if (source_list[j].id == result_set[i][1])
            {
                source_num = j;
                break;
            }
        if (source_num < 0)
        {
            wxLogMessage("Field definition for a non-existent source");
            return false;
        }

        // Save the information about the new field.

        gendat_source_field new_field;

        new_field.name        = result_set[i][2];
        new_field.description = result_set[i][3];
        new_field.db_field    = result_set[i][4];

        // Add the new field to the source.

        source_list[source_num].field_list.push_back(new_field);

    }

    return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of sources.
///
////////////////////////////////////////////////////////////////////////////////////////////////////


int gendat_source_list::num_sources () const
{
    return source_list.size();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of field definitions for a GenDat source
///
/// \param[in]  source_num   Source number
///
/// \return     number of fields
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////


int gendat_source_list::num_fields (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].field_list.size();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get a GenDat source name
///
/// \param[in]  source_num   Source number
///
/// \return     source name
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////


std::string gendat_source_list::get_name (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].name;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get a GenDat source description
///
/// \param[in]  source_num   Source number
///
/// \return     source description
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////


std::string gendat_source_list::get_description (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].description;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get database table of a GenDat source
///
/// \param[in]  source_num   Source number
///
/// \return     database table
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string gendat_source_list::get_db_table (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].db_table;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Find the parent of a GenDat source
///
/// \param[in]  source_num   Source number
///
/// \return     source number of the parent or -1 if there is no parent
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

int gendat_source_list::get_parent (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].my_parent;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Find the children of a GenDat source
///
/// \param[in]  source_num   Source number
///
/// \return     source children
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<int> gendat_source_list::get_children (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].my_children;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the field descriptions for a GenDat source
///
/// This member function provides readonly access to the definitions of the
/// fields in the database table that is linked to the specified GenDat source.
///
/// \param[in]  source_num   Source number
///
/// \return a vector of field descriptions
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<gendat_source_field> gendat_source_list::get_fields (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].field_list;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Make sure a GenDat source number is valid
///
/// \param[in]  source_num   Source number
///
/// \exception std::logic_error thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gendat_source_list::test_source_num (int source_num)
{
    if (source_num < 0 || source_num >= num_sources())
        throw std::logic_error("GenDat source number is out of range");
}
