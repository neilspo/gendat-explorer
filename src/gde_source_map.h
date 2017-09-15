///
/// \file
///

#ifndef GDE_SOURCE_MAP_H_INCLUDED
#define GDE_SOURCE_MAP_H_INCLUDED

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


enum class gde_field_type
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

    gde_relation    fam_rel         (int source_num, int field_num);
    std::string     fam_rel_text    (int source_num, int field_num);
    gde_field_type  field_type      (int source_num, int field_num);
    std::string     field_type_text (int source_num, int field_num);


private:

    std::vector<std::string> split (const std::string& input, const std::string& regex);

    struct fld_code_meaning
    {
        gde_relation    relation;
        gde_field_type  type = gde_field_type::UNKNOWN;
    };

    std::vector <std::vector <fld_code_meaning>> field_code_meanings;
};

#endif // GDE_SOURCE_MAP_H_INCLUDED
