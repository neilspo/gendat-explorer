///
/// \file
///

#ifndef GDE_SOURCE_MAP_H
#define GDE_SOURCE_MAP_H

#include <vector>

#include "db_map.h"

///
/// \brief Family relation codes.
///

enum class gde_relation
{
    UNDEFINED = 0,
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

///
/// \brief Data tags
///
/// These are the allowed tags that can be used to signify various kinds
/// of genealogical data. For the most part, these match the GEDCOM standard tags.
///

enum class gde_data_tag
{
    UNDEFINED,
    SURN,       ///< surname
    GIVN,       ///< given names(s)
    NAME,       ///< name
    BIRT,       ///< birth
    BAPM,       ///< baptism
    CONF,       ///< confirmation
    DEAT,       ///< death
    BURI,       ///< burial
    MARR,       ///< marriage
    DIV,        ///< divorce
    SEX,        ///< sex
    AGE,        ///< age
    RESI,       ///< residence
    OCCU,       ///< occupation
    NOTE,       ///< note
    CENS,       ///< census
    WILL,       ///< will (legal document)
    DATE,       ///< date
    PLAC,       ///< place
    XREF,       ///< GEDCOM cross-reference
    INDI,       ///< GEDCOM individual
    KEY,        ///< database primary key
    STATUS,     ///< marital status
    YEAR,       ///< year
    MONTH,      ///< month
    DAY,        ///< day
    COMMUNITY,  ///< community
    COUNTY,     ///< county
    CEMETERY,   ///< cemetery
    INSCRIPTION ///< headstone inscription
};

std::string data_tag_text (gde_data_tag data_tag);


class gde_source_map : public db_map
{
public:
    void            load_defs        (database &db, std::string src_defs, std::string fld_defs);

    gde_data_tag    src_type         (int source_num) const;

    gde_relation    fam_rel          (int source_num, int field_num) const;
    std::string     fam_rel_text     (int source_num, int field_num) const;
    gde_data_tag    event_type       (int source_num, int field_num) const;
    gde_data_tag    fact_type        (int source_num, int field_num) const;
    gde_data_tag    fact_type_mod    (int source_num, int field_num) const;

private:

    struct gde_field_def
    {
        bool           field_code_ok  = false;
        gde_relation   fam_relation   = gde_relation::UNDEFINED;
        gde_data_tag   event          = gde_data_tag::UNDEFINED;
        gde_data_tag   fact           = gde_data_tag::UNDEFINED;
        gde_data_tag   fact_mod       = gde_data_tag::UNDEFINED;
    };

    struct gde_source_def
    {
        bool                        source_code_ok = false;
        gde_data_tag                source_type    = gde_data_tag::UNDEFINED;
        std::vector<gde_field_def>  gde_field_list;
    };

    std::vector<gde_source_def>     gde_source_list;

    // A couple of private utility functions.

    std::vector<std::string> split (const std::string& input, const std::string& regex);
    void test_inputs (int source_num, int field_num) const;
};



class gde_search_map
{
public:
    gde_search_map(const gde_source_map& source_map);

    void add_source (gde_data_tag src_type);

    void req_field  (gde_relation fld_fam_rel,
                     gde_data_tag fld_event,
                     gde_data_tag fld_fact,
                     gde_data_tag fld_fact_mod);

    void opt_field  (gde_relation fld_fam_rel,
                     gde_data_tag fld_event,
                     gde_data_tag fld_fact,
                     gde_data_tag fld_fact_mod);

    bool get_field  (int index, int& source, std::vector<int>& field);

private:
    const gde_source_map&     my_source_map;

    std::vector<bool>              src_selected;
    std::vector<std::vector<bool>> fld_selected;
};

#endif
