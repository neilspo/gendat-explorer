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
/// \brief Constructor
///
/// \param[in]  source_map   database map object
///
////////////////////////////////////////////////////////////////////////////////////////////////////


gde_source_map::gde_source_map (const db_map& source_map)
{
    // Define the possible GenDAT source types.

    std::unordered_map <std::string, gde_data_tag> source_type_map;
    source_type_map["BIRT"] = gde_data_tag::BIRT;
    source_type_map["BAPM"] = gde_data_tag::BAPM;
    source_type_map["DEAT"] = gde_data_tag::DEAT;
    source_type_map["BURI"] = gde_data_tag::BURI;
    source_type_map["MARR"] = gde_data_tag::MARR;

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

    std::unordered_map <std::string, gde_data_tag> field_type_map;
    field_type_map["SURN"] = gde_data_tag::SURN;
    field_type_map["GIVN"] = gde_data_tag::GIVN;
    field_type_map["NAME"] = gde_data_tag::NAME;
    field_type_map["BIRT"] = gde_data_tag::BIRT;
    field_type_map["BAPM"] = gde_data_tag::BAPM;
    field_type_map["DEAT"] = gde_data_tag::DEAT;
    field_type_map["BURI"] = gde_data_tag::BURI;
    field_type_map["MARR"] = gde_data_tag::MARR;
    field_type_map["SEX" ] = gde_data_tag::SEX;
    field_type_map["AGE" ] = gde_data_tag::AGE;
    field_type_map["RESI"] = gde_data_tag::RESI;
    field_type_map["OCCU"] = gde_data_tag::OCCU;
    field_type_map["NOTE"] = gde_data_tag::NOTE;

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

    //-------------------------------------------------------------------------
    // Interpret the meanings of the source codes.
    //-------------------------------------------------------------------------

    source_type.resize(num_sources);
    for (int i=0; i<num_sources; i++)
    {
        auto iter = source_type_map.find(source_map.src_code(i));
        if (iter == source_type_map.end())
            source_type[i]  = gde_data_tag::UNKNOWN;
        else
            source_type[i] = iter->second;
    }

    //-------------------------------------------------------------------------
    // Interpret the meanings of the field codes.
    //-------------------------------------------------------------------------

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

                if (!tokens.empty())
                {
                    // Read the GenDat primary field type.

                    auto iter_2 = field_type_map.find(tokens[0]);
                    if (iter_2 == field_type_map.end())
                        field_code_meanings[i][j].type = gde_data_tag::UNKNOWN;
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





gde_data_tag gde_source_map::src_type (int source_num)
{
    return source_type[source_num];
}



std::string gde_source_map::src_type_text(int source_num)
{
    return data_tag_to_text(src_type(source_num));
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



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get primary field type
///
/// This member function returns the primary field type of the specified field.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     field type
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::field_type (int source_num, int field_num)
{
    return field_code_meanings[source_num][field_num].type;
}



std::string gde_source_map::field_type_text(int source_num, int field_num)
{
    return data_tag_to_text(field_type(source_num, field_num));
}





std::string gde_source_map::data_tag_to_text(gde_data_tag data_tag)
{
    switch (data_tag)
    {
    case gde_data_tag::SURN:
        return "Surname";
    case gde_data_tag::GIVN:
        return "Given Name(s)";
    case gde_data_tag::NAME:
        return "Name";
    case gde_data_tag::BIRT:
        return "Birth";
    case gde_data_tag::BAPM:
        return "Baptism";
    case gde_data_tag::DEAT:
        return "Death";
    case gde_data_tag::BURI:
        return "Burial";
    case gde_data_tag::MARR:
        return "Marriage";
    case gde_data_tag::SEX:
        return "Sex";
    case gde_data_tag::AGE:
        return "Age";
    case gde_data_tag::RESI:
        return "Residence";
    case gde_data_tag::OCCU:
        return "Occupation";
    case gde_data_tag::NOTE:
        return "Note";
    default:
        return "";
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Split a string into a set of tokens
///
/// \param[in]  input        String to be parsed
/// \param[in]  regex        Regular expression to be used to find the separator character(s)
///
/// \return     vector of tokens
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> gde_source_map::split(const std::string& input, const std::string& regex)
{
    std::regex re(regex);
    std::sregex_token_iterator
    first{input.begin(), input.end(), re, -1},
          last;
    return {first, last};
}
