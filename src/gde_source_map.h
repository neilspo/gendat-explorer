///
/// \file
///

#ifndef GDE_SOURCE_MAP_H
#define GDE_SOURCE_MAP_H

#include "db_map.h"

enum class gde_relation
{
    UNKNOWN = 0,
    FATHER,
    MOTHER,
    SPOUSE,
    BRIDE,
    BRIDE_FATHER,
    BRIDE_MOTHER,
    GROOM,
    GROOM_FATHER,
    GROOM_MOTHER
};


enum class gde_data_tag
{
    UNKNOWN = 0,
    SURN,
    GIVN,
    NAME,
    BIRT,
    BAPM,
    DEAT,
    BURI,
    MARR,
    SEX,
    AGE,
    RESI,
    OCCU,
    NOTE,

    DATE,
    PLAC
};

class gde_source_map
{
public:
    gde_source_map (const db_map& source_map);

    gde_data_tag    src_type      (int source_num);
    std::string     src_type_text (int source_num);

    gde_relation    fam_rel          (int source_num, int field_num);
    std::string     fam_rel_text     (int source_num, int field_num);
    gde_data_tag    field_type       (int source_num, int field_num);
    std::string     field_type_text  (int source_num, int field_num);




private:

    struct fld_code_meaning
    {
        gde_relation  relation;
        gde_data_tag  type = gde_data_tag::UNKNOWN;
    };

    // Private variables

    std::vector <std::vector <fld_code_meaning>> field_code_meanings;
    std::vector <gde_data_tag>                   source_type;

    // A couple of private utility functions.

    std::vector<std::string> split (const std::string& input, const std::string& regex);
    std::string data_tag_to_text (gde_data_tag data_tag);
};

#endif
