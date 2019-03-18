///
/// \file
///

#ifndef GDE_SEARCH_MAP_H
#define GDE_SEARCH_MAP_H

#include <vector>

#include "gde_source_map.h"



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
