///
/// \file
///



#ifndef GDE_SOURCE_H
#define GDE_SOURCE_H

#include <string>
#include <vector>

#include "database.h"


enum src_type
{
    UNKNOWN = 0,
    BIRT,
    MARR,
    DEAT
};

class gendat_source_list
{
public:
    bool load_defs (database &db);

private:

    class gendat_source
    {
    public:
        std::string   name;
        std::string   description;
        src_type      type;
        std::string   table;
        bool          writable;
    };

    std::vector<gendat_source> source_list;
};



#endif

