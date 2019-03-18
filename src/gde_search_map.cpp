///
/// \class gde_search_map gde_search_map.h
///
/// \brief Handles a GenDat search map
///
/// This class ...
///

#include <iostream>

#include "gde_search_map.h"


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
    // For all of the selected sources, ...

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
