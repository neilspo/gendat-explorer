///
/// \file
///

#ifndef W_SEARCH_H
#define W_SEARCH_H

#include <string>
#include <vector>

#include "database.h"

class w_search
{
public:
    void add_field (std::string db_field_name, std::string search_string);
    void execute_search (database &db, std::string base_query, int max_rows=0);

private:
    int                      num_args = 0;
    std::vector<std::string> arg_list;
    std::string              where_clause;
};

#endif
