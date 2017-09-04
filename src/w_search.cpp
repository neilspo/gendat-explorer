///
/// \class w_search w_search.h
///
/// \brief Provides database search with wildcards
///

#include "w_search.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Add a field to the WHERE clause
///
/// \param[in] db_field_name   field name in the database
/// \param[in] search_string   string to be used as the search pattern
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void w_search::add_field (std::string db_field_name, std::string search_string)
{
    if (!search_string.empty())
    {
        if(++num_args == 1)
            where_clause = " WHERE ";
        else
            where_clause = where_clause + " AND ";

    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Build and execute the search query.
///
/// \param[in] db              database connection, which must currently be open
/// \param[in] base_query      base SQL query for the search
/// \param[in] max_rows        maximum number of rows that will be obtained from the database
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void execute_search (database &db, std::string base_query, int max_rows)
{

}
