///
/// \class gde_source_map gde_source_map.h
///
/// \brief Handles information about GenDat data sources
///


#include <regex>
#include <unordered_map>
#include <stdexcept>

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
    source_type_map["CONF"] = gde_data_tag::CONF;
    source_type_map["DEAT"] = gde_data_tag::DEAT;
    source_type_map["BURI"] = gde_data_tag::BURI;
    source_type_map["MARR"] = gde_data_tag::MARR;
    source_type_map["DIV" ] = gde_data_tag::DIV;
    source_type_map["CENS"] = gde_data_tag::CENS;
    source_type_map["WILL"] = gde_data_tag::WILL;

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

    // Define the possible GenDat event types. These are milestones in
    // a person's life, which occur at a specific time and place.

    std::unordered_map <std::string, gde_data_tag> event_type_map;
    event_type_map["BIRT"] = gde_data_tag::BIRT;
    event_type_map["BAPM"] = gde_data_tag::BAPM;
    event_type_map["CONF"] = gde_data_tag::CONF;
    event_type_map["DEAT"] = gde_data_tag::DEAT;
    event_type_map["BURI"] = gde_data_tag::BURI;
    event_type_map["MARR"] = gde_data_tag::MARR;
    event_type_map["DIV" ] = gde_data_tag::DIV;

    // Define the possible GenDat fact types. These tags refer to some
    // attribute of an event or person.

    std::unordered_map <std::string, gde_data_tag> fact_type_map;
    fact_type_map["SURN"] = gde_data_tag::SURN;
    fact_type_map["GIVN"] = gde_data_tag::GIVN;
    fact_type_map["NAME"] = gde_data_tag::NAME;
    fact_type_map["SEX" ] = gde_data_tag::SEX;
    fact_type_map["AGE" ] = gde_data_tag::AGE;
    fact_type_map["DATE"] = gde_data_tag::DATE;
    fact_type_map["PLAC"] = gde_data_tag::PLAC;
    fact_type_map["RESI"] = gde_data_tag::RESI;
    fact_type_map["OCCU"] = gde_data_tag::OCCU;
    fact_type_map["NOTE"] = gde_data_tag::NOTE;

    // If there are no defined sources, then there is nothing to do.

    int num_sources = source_map.num_sources();

    if (num_sources <= 0)
        return;

    // Initialize the searchable field lookup table.

    searchable_field_lookup.resize(num_sources);
    for (int i=0; i<num_sources; i++)
    {
        int num_fields = source_map.num_fields(i);
        searchable_field_lookup[i].resize(num_fields);
        for (int j=0; j<num_fields; j++)
        {
            searchable_field_lookup[i][j] = -1;
        }
    }

    //-------------------------------------------------------------------------
    // Interpret the meanings of the type codes for all of the sources.
    //-------------------------------------------------------------------------

    source_type.resize(num_sources);
    for (int i=0; i<num_sources; i++)
    {
        auto iter = source_type_map.find(source_map.src_code(i));
        if (iter == source_type_map.end())
            source_type[i]  = gde_data_tag::UNDEFINED;
        else
            source_type[i] = iter->second;
    }

    //-------------------------------------------------------------------------
    // Look at every field definition and interpret the meanings of all valid
    // source and field code combinations.
    //-------------------------------------------------------------------------

    for (int i=0; i<num_sources; i++)
    {
        int num_fields = source_map.num_fields(i);
        for (int j=0; j<num_fields; j++)
        {
            std::string fld_code = source_map.fld_code(i,j);
            if (!fld_code.empty())
            {
                searchable_field s_field;
                s_field.source_num = i;
                s_field.field_num  = j;

                // Split apart the separate parts of the field code into a vector of tokens.

                std::vector<std::string> tokens = split(fld_code, "\\_");

                // Read the family relationship part of the field code, if there is one.

                auto iter_1 = rel_map.find(tokens[0]);
                if (iter_1 != rel_map.end())
                {
                    s_field.fam_relation = iter_1->second;
                    tokens.erase(tokens.begin());
                }

                // Read the GenDat event type, if there is one.

                auto iter_2 = event_type_map.find(tokens[0]);
                if (iter_2 != event_type_map.end())
                {
                    s_field.event = iter_2->second;
                    tokens.erase(tokens.begin());
                }

                // Read the GenDat fact type. This is the only mandatory part of
                // a GenDat field code.

                auto iter_3 = fact_type_map.find(tokens[0]);
                if (iter_3 != fact_type_map.end())
                {
                    s_field.fact = iter_3->second;
                    tokens.erase(tokens.begin());
                }

                // Read the GenDat fact type modifier, if the is one.




                // If this field code is a valid, then save it in the list of
                // searchable fields.

                searchable_field_lookup[i][j] = searchable_field_list.size();
                searchable_field_list.push_back(s_field);
            }
        }
    }



}




gde_data_tag gde_source_map::src_type (int source_num)
{
    if (source_num < 0 || source_num >= (int)source_type.size())
        throw std::out_of_range("Source number in gde_source_map::src_type is out of range");
    return source_type[source_num];
}



std::string gde_source_map::src_type_text(int source_num)
{
    return data_tag_text(src_type(source_num));
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
    return searchable_field_list[searchable_field_lookup[source_num][field_num]].fam_relation;
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
    switch (fam_rel(source_num, field_num))
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
/// \brief Get GenDat event type
///
/// This member function returns the primary field type of the specified field.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     event type
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::event_type(int source_num, int field_num)
{
    return searchable_field_list[searchable_field_lookup[source_num][field_num]].event;
}


gde_data_tag gde_source_map::fact_type(int source_num, int field_num)
{
    return searchable_field_list[searchable_field_lookup[source_num][field_num]].fact;
}




//std::string gde_source_map::field_type_text(int source_num, int field_num)
//{
//    return data_tag_text(field_type(source_num, field_num));
//}





std::string data_tag_text(gde_data_tag data_tag)
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
    case gde_data_tag::CENS:
        return "Census";
    case gde_data_tag::WILL:
        return "Will";
    case gde_data_tag::DATE:
        return "Date";
    case gde_data_tag::PLAC:
        return "Place";

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
