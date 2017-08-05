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

    std::vector< std::vector< std::string >> result_set;
    unsigned int num_rows;
    unsigned int num_cols;

    // Try to read the GenDat source definitions from the database.
    // If this fails, then report the error and return false.

    query = "SELECT name, description, type, db_table FROM z_sour";

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
        source_list[i].name        = result_set[i][0];
        source_list[i].description = result_set[i][1];
        if (result_set[i][2] == "BIRT") source_list[i].type = BIRT;
        if (result_set[i][2] == "DEAT") source_list[i].type = DEAT;
        if (result_set[i][2] == "MARR") source_list[i].type = MARR;


        std::cout << "source: " << result_set[i][2] << ", " << source_list[i].type << std::endl;
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
