#include <regex>
#include "gde_source_map.h"


gde_source_map::gde_source_map (const db_map& source_map)
{

    int num_sources = source_map.num_sources();

    // If there are no defined sources, then there is nothing to do.

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
            std::string fld_code = source_map.fld_code(i,j);

            // Read the family relationship code.

            if (std::regex_match(fld_code, std::regex("^F\\_.*")))
            {
                field_code_meanings[i][j].relation = FATHER;
                fld_code.erase(0,2);
            }
            else if (std::regex_match(fld_code, std::regex("^M\\_.*")))
            {
                field_code_meanings[i][j].relation = MOTHER;
                fld_code.erase(0,2);
            }
            else if (std::regex_match(fld_code, std::regex("^S\\_.*")))
            {
                field_code_meanings[i][j].relation = SPOUSE;
                fld_code.erase(0,2);
            }
            else if (std::regex_match(fld_code, std::regex("^B\\_.*")))
            {
                field_code_meanings[i][j].relation = BRIDE;
                fld_code.erase(0,2);
            }
            else if (std::regex_match(fld_code, std::regex("^BF\\_.*")))
            {
                field_code_meanings[i][j].relation = BRIDE_FATHER;
                fld_code.erase(0,3);
            }
            else if (std::regex_match(fld_code, std::regex("^BM\\_.*")))
            {
                field_code_meanings[i][j].relation = BRIDE_MOTHER;
                fld_code.erase(0,3);
            }
            else if (std::regex_match(fld_code, std::regex("^G\\_.*")))
            {
                field_code_meanings[i][j].relation = GROOM;
                fld_code.erase(0,2);
            }
            else if (std::regex_match(fld_code, std::regex("^GF\\_.*")))
            {
                field_code_meanings[i][j].relation = GROOM_FATHER;
                fld_code.erase(0,3);
            }
            else if (std::regex_match(fld_code, std::regex("^GM\\_.*")))
            {
                field_code_meanings[i][j].relation = GROOM_MOTHER;
                fld_code.erase(0,3);
            }
            else
            {
                field_code_meanings[i][j].relation = INDIVIDUAL;
            }

            // Read the GenDat field types.

            if (std::regex_match(fld_code, std::regex("^SURN.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::SURN;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^GIVN.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::GIVN;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^NAME.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::NAME;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^BIRT.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::BIRT;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^BAPM.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::BAPM;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^DEAT.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::DEAT;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^BURI.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::BURI;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^MARR.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::MARR;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^SEX.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::SEX;
                fld_code.erase(0,3);
            }
            else if (std::regex_match(fld_code, std::regex("^AGE.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::AGE;
                fld_code.erase(0,3);
            }
            else if (std::regex_match(fld_code, std::regex("^RESI.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::RESI;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^OCCU.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::OCCU;
                fld_code.erase(0,4);
            }
            else if (std::regex_match(fld_code, std::regex("^NOTE.*")))
            {
                field_code_meanings[i][j].type = gde_field_type::NOTE;
                fld_code.erase(0,4);
            }
            else
            {
                field_code_meanings[i][j].type = gde_field_type::UNKNOWN;
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
    case FATHER:
        return "Father";
    case MOTHER:
        return "Mother";
    case SPOUSE:
        return "Spouse";
    case BRIDE:
        return "Bride";
    case BRIDE_FATHER:
        return "Bride's Father";
    case BRIDE_MOTHER:
        return "Bride's Mother";
    case GROOM:
        return "Groom";
    case GROOM_FATHER:
        return "Groom's Father";
    case GROOM_MOTHER:
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

