///
/// \class gde_source_map gde_source_map.h
///
/// \brief Handles information about GenDat data sources
///
/// This class extends the `db_map` class by defining a set of source and field codes
/// that describe the types of data in the GenDat database.
///
/// Here, the term <em>GenDat source</em> will refer to any database table that
/// holds a set of data records that were all extracted from one particular type
/// of genealogical source. Examples would be data transcribed from official birth
/// records for some province, from the marriage records from some church, or
/// from an official census. See `gde_source_map::src_type()` for the possible
/// GenDat source types.
///
/// The GenDat field codes describe the meanings of the database fields. These
/// field codes are composed of some or all of the following components:
///
/// - family relationship type (see `gde_source_map::fam_rel()`)
/// - event type (see `gde_source_map::event_type()`)
/// - fact type (see `gde_source_map::fact_type()`)
/// - fact type modifier (see `gde_source_map::fact_type_mod()`)
///


#include <regex>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

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

void gde_source_map::load_defs(database &db, std::string src_defs, std::string fld_defs)
{
    // Run the overloaded version of this function from the base class.

    db_map::load_defs (db, src_defs, fld_defs);

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
    fact_type_map["XREF"] = gde_data_tag::XREF;
    fact_type_map["KEY" ] = gde_data_tag::KEY;
    fact_type_map["STATUS"     ] = gde_data_tag::STATUS;
    fact_type_map["CEMETERY"   ] = gde_data_tag::CEMETERY;
    fact_type_map["INSCRIPTION"] = gde_data_tag::INSCRIPTION;

    // Define the possible GenDat fact type modifiers.

    std::unordered_map <std::string, gde_data_tag> fact_type_mod_map;
    fact_type_mod_map["Y"          ] = gde_data_tag::YEAR;
    fact_type_mod_map["M"          ] = gde_data_tag::MONTH;
    fact_type_mod_map["D"          ] = gde_data_tag::DAY;
    fact_type_mod_map["COMMUNITY"  ] = gde_data_tag::COMMUNITY;
    fact_type_mod_map["COUNTY"     ] = gde_data_tag::COUNTY;
    fact_type_mod_map["INDI"       ] = gde_data_tag::INDI;

    // If there are no defined sources, then there is nothing to do.

    if (num_sources() <= 0)
    return;

    // Initialize the source and field lists.

    gde_source_list.resize(num_sources());
    for (int i=0; i<num_sources(); i++)
        gde_source_list[i].gde_field_list.resize(num_fields(i));


    //-------------------------------------------------------------------------
    // Interpret the meanings of the type codes for all of the sources.
    //-------------------------------------------------------------------------

    for (int i=0; i<num_sources(); i++)
    {
        auto iter = source_type_map.find(src_code(i));
        if (iter != source_type_map.end())
        {
            gde_source_list[i].source_code_ok = true;
            gde_source_list[i].source_type    = iter->second;
        }
    }

    //-------------------------------------------------------------------------
    // Look at every field definition and interpret the meanings of all valid
    // source and field code combinations.
    //-------------------------------------------------------------------------

    for (int i=0; i<num_sources(); i++)
    {
        for (int j=0; j<num_fields(i); j++)
        {
            std::string my_fld_code = fld_code(i,j);
            if (!my_fld_code.empty())
            {
                gde_field_def s_field;

                // Split apart the separate parts of the field code into a vector of tokens.

                std::vector<std::string> tokens = split(my_fld_code, "\\_");

                // Read the family relationship part of the field code, if there is one.

                auto iter_1 = rel_map.find(tokens[0]);
                if (iter_1 != rel_map.end())
                {
                    s_field.fam_relation = iter_1->second;
                    tokens.erase(tokens.begin());
                }

                // Read the GenDat event type, if there is one.

                if (!tokens.empty())
                {
                    auto iter_2 = event_type_map.find(tokens[0]);
                    if (iter_2 != event_type_map.end())
                    {
                        s_field.event = iter_2->second;
                        tokens.erase(tokens.begin());
                    }
                }

                // Read the GenDat fact type. This is the only mandatory part of
                // a GenDat field code.

                if (!tokens.empty())
                {
                    auto iter_3 = fact_type_map.find(tokens[0]);
                    if (iter_3 != fact_type_map.end())
                    {
                        s_field.fact = iter_3->second;
                        tokens.erase(tokens.begin());
                    }
                }

                // Read the GenDat fact type modifier, if the is one.

                if (!tokens.empty())
                {
                    auto iter_4 = fact_type_mod_map.find(tokens[0]);
                    if (iter_4 != fact_type_mod_map.end())
                    {
                        s_field.fact_mod = iter_4->second;
                        tokens.erase(tokens.begin());
                    }
                }

                // Now do some basic sanity tests on the field code.

                bool valid_field_code = true;

                //-----The field must describe some fact about something.

                if (s_field.fact == gde_data_tag::UNDEFINED)
                    valid_field_code = false;

                //-----There should be no unread tokens left.

                if (!tokens.empty())
                    valid_field_code = false;

                //-----Only marriages should have brides and grooms.

                if (gde_source_list[i].source_type!= gde_data_tag::MARR)
                {
                    switch (s_field.fam_relation)
                    {
                    case gde_relation::BRIDE:
                    case gde_relation::BRIDE_FATHER:
                    case gde_relation::BRIDE_MOTHER:
                    case gde_relation::GROOM:
                    case gde_relation::GROOM_FATHER:
                    case gde_relation::GROOM_MOTHER:
                        valid_field_code = false;
                    default:
                        break;
                    }
                }

                // If this field code is a valid, then save it in the list of
                // searchable fields.

                if (valid_field_code)
                {
                    gde_source_list[i].gde_field_list[j] = s_field;
                }
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get source type
///
/// This member function returns the data type tag of the specified GenDat source.
///
/// The currently defined GenDat source types are: birth, baptism, confirmation, death, burial,
/// marriage, divorce, census and will records.
///
/// \param[in]  source_num   source number
///
/// \return     source type
///
/// \exception std::out_of_range thrown if the source number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::src_type (int source_num) const
{
    if (source_num < 0 || source_num >= num_sources())
        throw std::out_of_range("Source number in gde_source_map::src_type is out of range");
    return gde_source_list[source_num].source_type;
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
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_relation gde_source_map::fam_rel (int source_num, int field_num) const
{
    test_inputs(source_num, field_num);
    return gde_source_list[source_num].gde_field_list[field_num].fam_relation;
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
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string gde_source_map::fam_rel_text(int source_num, int field_num) const
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
/// This member function returns the GenDat event type of the specified field.
///
/// These events are milestones in a person's life, which occur at a specific time and place.
/// The currently defined GenDat events are: birth, baptism, confirmation, death, burial,
/// marriage and divorce.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     event type
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::event_type(int source_num, int field_num) const
{
    test_inputs(source_num, field_num);
    return gde_source_list[source_num].gde_field_list[field_num].event;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get GenDat fact type
///
/// This member function returns the GenDat fact type of the specified field.
///
/// These fact types refer to some attribute of an event or person.
/// The currently defined fact types are: surname, given names(s), name, sex, age, date,
/// place, residence, occupation, note, XREF (cross-reference), database key, marital status,
/// cemetery and inscription.
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     fact type
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::fact_type(int source_num, int field_num) const
{
    test_inputs(source_num, field_num);
    return gde_source_list[source_num].gde_field_list[field_num].fact;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get GenDat fact type modifier
///
/// This member function returns the GenDat fact type modifier of the specified field.
///
/// The currently defined fact type modifiers are: year, month, day, community, county and
/// INDI (a type of cross-reference).
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \return     fact type modifier
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_data_tag gde_source_map::fact_type_mod(int source_num, int field_num) const
{
    test_inputs(source_num, field_num);
    return gde_source_list[source_num].gde_field_list[field_num].fact_mod;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get text description of a GenDat data tag
///
/// \param[in]  data_tag     GenDat data tag
///
/// \return     text description of the data tag
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    case gde_data_tag::CONF:
        return "Confirmation";
    case gde_data_tag::DEAT:
        return "Death";
    case gde_data_tag::BURI:
        return "Burial";
    case gde_data_tag::MARR:
        return "Marriage";
    case gde_data_tag::DIV:
        return "Divorce";
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
    case gde_data_tag::XREF:
        return "Cross Ref.";
    case gde_data_tag::INDI:
        return "Individual";
    case gde_data_tag::KEY:
        return "Pri. Key";
    case gde_data_tag::STATUS:
        return "Marital Status";
    case gde_data_tag::YEAR:
        return "Year";
    case gde_data_tag::MONTH:
        return "Month";
    case gde_data_tag::DAY:
        return "Day";
    case gde_data_tag::COMMUNITY:
        return "Community";
    case gde_data_tag::COUNTY:
        return "County";
    case gde_data_tag::CEMETERY:
        return "Cemetery";
    case gde_data_tag::INSCRIPTION:
        return "Inscription";

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



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Make sure the source and field numbers are valid
///
/// \param[in]  source_num   Source number
/// \param[in]  field_num    Field number
///
/// \exception std::out_of_range thrown the source or field number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gde_source_map::test_inputs (int source_num, int field_num) const
{
    if (source_num < 0 || source_num >= num_sources() ||
            (field_num < 0 || field_num >= num_fields(source_num)))
        throw std::out_of_range("Source or field number in gde_source_map:: is out of range");
}





//--------------------------------------------------------------------------------------------------

///
/// \class gde_search_map gde_source_map.h
///
/// \brief Handles a GenDat search map
///
/// This class ...
///



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param[in]  source_map   object containing the GenDat source definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gde_search_map::gde_search_map(const gde_source_map& source_map) :
    my_source_map(source_map)
{
    unsigned int num_src = my_source_map.num_sources();

    src_selected.resize(num_src, false);
    fld_selected.resize(num_src);
    for (unsigned int src=0; src<num_src; src++)
        fld_selected[src].resize(my_source_map.num_fields(src), false);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Add GenDat sources of a particular type
///
/// This member function adds all GenDat sources of the given type to the search. It can be called
/// multiple times to include more that one type of source.
///
/// \param[in]  src_type     Source type
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gde_search_map::add_source (gde_data_tag src_type)
{
    for (int i=0; i<my_source_map.num_sources(); i++)
        if (my_source_map.src_type(i) == src_type)
            src_selected[i] = true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Add required fields to database search
///
/// This member function ...
///
/// \param[in]  fld_fam_rel    Family relationship type
/// \param[in]  fld_event      Event type
/// \param[in]  fld_fact       Fact type
/// \param[in]  fld_fact_mod   Fact type modifier
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gde_search_map::req_field (gde_relation fld_fam_rel,
                                gde_data_tag fld_event,
                                gde_data_tag fld_fact,
                                gde_data_tag fld_fact_mod)

{
    // For all of the selected sources,

    for (int i=0; i<my_source_map.num_sources(); i++)
        if (src_selected[i])
            for (int j=0; j<my_source_map.num_fields(i); j++)
                if(my_source_map.fact_type(i,j) == fld_fact)
                {
                    std::cout << my_source_map.src_db_table(i) << ", " << my_source_map.fld_db_name(i,j) << std::endl;
                }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Add optional fields to database search
///
/// This member function ...
///
/// \param[in]  fld_fam_rel    Family relationship type
/// \param[in]  fld_event      Event type
/// \param[in]  fld_fact       Fact type
/// \param[in]  fld_fact_mod   Fact type modifier
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gde_search_map::opt_field (gde_relation fld_fam_rel,
                                gde_data_tag fld_event,
                                gde_data_tag fld_fact,
                                gde_data_tag fld_fact_mod)
{

}
