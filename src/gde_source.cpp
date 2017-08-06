///
/// \class gde_source gde_source.h
///
/// \brief Encapsulates information about a GenDat source
///
/// This class handles ...
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
/// \param[in]  query   Database query to use to load this level
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

    // Set up the source hierarchy.

    for (unsigned int i=0; i<num_rows; i++)
    {
        if (!source_list[i].derived_from.empty())
        {
            for (unsigned int j=0; j<num_rows; j++)
            {
                if (source_list[i].derived_from == source_list[j].id)
                {
                    source_list[j].my_children.push_back(i);
                }
            }

        }
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
/// \brief Get a GenDat source name
///
/// \param[in]  source_num   Source number
///
/// \return     source name
///
////////////////////////////////////////////////////////////////////////////////////////////////////


std::string gendat_source_list::get_name (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].name;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get a GenDat source parent
///
/// \param[in]  source_num   Source number
///
/// \return     source parent
///
////////////////////////////////////////////////////////////////////////////////////////////////////


std::string gendat_source_list::get_parent_id (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].derived_from;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get a GenDat source children
///
/// \param[in]  source_num   Source number
///
/// \return     source children
///
////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<int> gendat_source_list::get_children (int source_num)
{
    test_source_num (source_num);
    return source_list[source_num].my_children;
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
