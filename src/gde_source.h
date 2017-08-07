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
    bool           load_defs   (database &db);
    int            num_sources () const;

    std::string get_name        (int source_num);
    std::string get_description (int source_num);

    int              get_parent    (int source_num);
    std::vector<int> get_children  (int source_num);

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
        src_type      type = UNKNOWN;

        int              my_parent;
        std::vector<int> my_children;
    };

    std::vector<gendat_source> source_list;

    void test_source_num (int source_num);
};

#endif

