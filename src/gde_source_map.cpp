///
/// \class gde_source_map gde_source_map.h
///
/// \brief Handles information about GenDat data sources
///


#include <regex>
#include <unordered_map>
#include "gde_source_map.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Load source and field definitions from database
///
/// \param[in]  db         database connection, which must currently be open
/// \param[in]  src_defs   database table with the source definitions
/// \param[in]  fld_defs   database table with the field definitions
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////


gde_source_map::gde_source_map (const db_map& source_map)
{
    // Define the possible GenDat family relation codes.

    std::unordered_map <std::string, gde_relation> rel_map;
    rel_map["F" ] = gde_relation::FATHER;
    rel_map["M" ] = gde_relation::MOTHER;
    rel_map["S" ] = gde_relation::SPOUSE;
    rel_map["B" ] = gde_relation::BRIDE;
    rel_map["BF"] = gde_relation::BRIDE_FATHER;
    rel_map["BM"] = gde_relation::BRIDE_MOTHER;
    rel_map["G" ] = gde_relation::GROOM;
    rel_map["GF"] = gde_relation::GROOM_FATHER;
    rel_map["GM"] = gde_relation::GROOM_MOTHER;

    // Define the possible GenDat primary field types.

    std::unordered_map <std::string, gde_field_type> type_map;
    type_map["SURN"] = gde_field_type::SURN;
    type_map["GIVN"] = gde_field_type::GIVN;
    type_map["NAME"] = gde_field_type::NAME;
    type_map["BIRT"] = gde_field_type::BIRT;
    type_map["BAPM"] = gde_field_type::BAPM;
    type_map["DEAT"] = gde_field_type::DEAT;
    type_map["BURI"] = gde_field_type::BURI;
    type_map["MARR"] = gde_field_type::MARR;
    type_map["SEX" ] = gde_field_type::SEX;
    type_map["AGE" ] = gde_field_type::AGE;
    type_map["RESI"] = gde_field_type::RESI;
    type_map["OCCU"] = gde_field_type::OCCU;
    type_map["NOTE"] = gde_field_type::NOTE;

    // If there are no defined sources, then there is nothing to do.

    int num_sources = source_map.num_sources();

    if (num_sources <= 0)
        return;

    // Resize the vector to hold the correct number of sources and fields.

    field_code_meanings.resize(num_sources);
    for (int i=0; i<num_sources; i++)
    {
        int num_fields = source_map.num_fields(i);
        if (num_fields > 0)
            field_code_meanings[i].resize(num_fields);
    }

    // Interpret the meanings of the field codes.

    for (int i=0; i<num_sources; i++)
    {
        int num_fields = source_map.num_fields(i);
        for (int j=0; j<num_fields; j++)
        {
            // Get the field code and make sure it's not empty.

            std::string fld_code = source_map.fld_code(i,j);

            if (!fld_code.empty())
            {
                // Split apart the separate parts of the field code into a vector of tokens.

                std::vector<std::string> tokens = split(fld_code, "\\_");

                // Read the GenDat family relationship code, if there is one.

                auto iter_1 = rel_map.find(tokens[0]);
                if (iter_1 == rel_map.end())
                    field_code_meanings[i][j].relation = gde_relation::UNKNOWN;
                else
                {
                    field_code_meanings[i][j].relation = iter_1->second;
                    tokens.erase(tokens.begin());
                }

                // Read the GenDat primary field type, if there is one.

                if (!tokens.empty())
                {

                    auto iter_2 = type_map.find(tokens[0]);
                    if (iter_2 == type_map.end())
                        field_code_meanings[i][j].type = gde_field_type::UNKNOWN;
                    else
                    {
                        field_code_meanings[i][j].type = iter_2->second;
                        tokens.erase(tokens.begin());
                    }

                    // Read the GenDat field type modifiers, if there are any.
                    //
                    // ***** To do. Currently, the only modifiers are PLAC and DATE.

                }
            }
        }
    }
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get family relationship type
///
/// This member function returns the family relationship of the person
/// referenced in the specified field.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     relationship
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_relation gde_source_map::fam_rel (int source_num, int field_num)
{
    return field_code_meanings[source_num][field_num].relation;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get family relationship text
///
/// This member function returns the text description of the family relationship of the person
/// referenced in the specified field.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     text description of the relationship
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string gde_source_map::fam_rel_text(int source_num, int field_num)
{
    switch (field_code_meanings[source_num][field_num].relation)
    {
    case gde_relation::FATHER:
        return "Father";
    case gde_relation::MOTHER:
        return "Mother";
    case gde_relation::SPOUSE:
        return "Spouse";
    case gde_relation::BRIDE:
        return "Bride";
    case gde_relation::BRIDE_FATHER:
        return "Bride's Father";
    case gde_relation::BRIDE_MOTHER:
        return "Bride's Mother";
    case gde_relation::GROOM:
        return "Groom";
    case gde_relation::GROOM_FATHER:
        return "Groom's Father";
    case gde_relation::GROOM_MOTHER:
        return "Groom's Mother";
    default:
        return "";
    }
}



std::string gde_source_map::field_type_text(int source_num, int field_num)
{
    switch (field_code_meanings[source_num][field_num].type)
    {
    case gde_field_type::SURN:
        return "Surname";
    case gde_field_type::GIVN:
        return "Given Name(s)";
    case gde_field_type::NAME:
        return "Name";
    case gde_field_type::BIRT:
        return "Birth";
    case gde_field_type::BAPM:
        return "Baptism";
    case gde_field_type::DEAT:
        return "Death";
    case gde_field_type::BURI:
        return "Burial";
    case gde_field_type::MARR:
        return "Marriage";
    case gde_field_type::SEX:
        return "Sex";
    case gde_field_type::AGE:
        return "Age";
    case gde_field_type::RESI:
        return "Residence";
    case gde_field_type::OCCU:
        return "Occupation";
    case gde_field_type::NOTE:
        return "Note";
    default:
        return "";
    }
}




std::vector<std::string> gde_source_map::split(const std::string& input, const std::string& regex)
{
    std::regex re(regex);
    std::sregex_token_iterator
    first{input.begin(), input.end(), re, -1},
          last;
    return {first, last};
}
