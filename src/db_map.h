///
/// \file
///



#ifndef DB_MAP_H
#define DB_MAP_H

#include <string>
#include <vector>

#include "database.h"


class db_map
{
public:
    void             load_defs       (database &db, std::string src_defs, std::string fld_defs);
    int              num_sources     () const;
    int              num_fields      (int source_num) const;

    std::string      src_name        (int source_num) const;
    std::string      src_description (int source_num) const;
    std::string      src_db_table    (int source_num) const;
    int              src_parent      (int source_num) const;
    std::vector<int> src_children    (int source_num) const;

    std::string      fld_name        (int source_num, int field_num) const;
    std::string      fld_code        (int source_num, int field_num) const;
    std::string      fld_db_name     (int source_num, int field_num) const;

private:

    class field_def
    {
    public:
        std::string   code;
        std::string   name;
        std::string   db_field;
        bool          writable;
    };

    class source_def
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

        // Allow a source hierarchy.

        int              my_parent;
        std::vector<int> my_children;

        // Define fields from this source that can used within GenDat Explorer.

        std::vector<field_def> field_list;
    };

    std::vector<source_def> source_list;

    void test_input  (int source_num) const;
    void test_inputs (int source_num, int field_num) const;
};

#endif

