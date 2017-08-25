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
    UNKNOWN_SRC_TYPE = 0,
    BIRT,
    MARR,
    DEAT
};

enum fld_type
{
    UNKNOWN_FLD_TYPE = 0,
    NAME,
    GIVN,
    SURN
};



///
/// \class gendat_source_field gde_source.h
///
/// \brief Handles information about field in a GenDat source
///

class gendat_source_field
{
public:

    std::string   get_name        () {return name;       }  ///< Get field name
    std::string   get_description () {return description;}  ///< Get field description
    std::string   get_db_field    () {return db_field;   }  ///< Get database field name

private:
    std::string   name;
    std::string   description;
    std::string   db_field;
    bool          writable;
    fld_type      type = UNKNOWN_FLD_TYPE;

    friend class gendat_source_list;
};




class gendat_source_list
{
public:
    bool             load_defs       (database &db);
    int              num_sources     () const;
    int              num_fields      (int source_num);

    std::string      get_name        (int source_num);
    std::string      get_description (int source_num);
    std::string      get_db_table    (int source_num);
    int              get_parent      (int source_num);
    std::vector<int> get_children    (int source_num);

    std::vector<gendat_source_field> get_fields (int source_num);

private:


    class gendat_source
    {
    public:
        std::string   id;
        std::string   name;
        std::string   description;
        std::string   version;
        std::string   db_table;
        std::string   repository;
        std::string   derived_from;
        bool          writable;
        src_type      type = UNKNOWN_SRC_TYPE;

        // Allow a source hierarchy.

        int              my_parent;
        std::vector<int> my_children;

        // Define fields from this source that can used within GenDat Explorer.

        std::vector<gendat_source_field> field_list;
    };

    std::vector<gendat_source> source_list;

    void test_source_num (int source_num);
};

#endif

