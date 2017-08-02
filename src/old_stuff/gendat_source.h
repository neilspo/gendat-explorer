///
/// \file gendat_source.h
///

#ifndef GENDAT_SOURCE_H
#define GENDAT_SOURCE_H

#include <string>
#include <vector>
#include "db_row_set.h"


class gsrc_sub_branch
{
public:
	gsrc_sub_branch();
	~gsrc_sub_branch();
	
	void load (std::string query);
private:
	bool                            not_loaded = true;
	db_row_set                      sub_branch_description_list;
	std::vector <gsrc_sub_branch *> sub_branche_list;
};


class gendat_source
{
public:
	gendat_source(std::string name);
	~gendat_source();

	void add_level(std::string query);
private:
	std::string                my_name;        // Name to be used for this GenDat source
	std::vector <std::string>  my_queries;     // Database queries
	gsrc_sub_branch            gsrc_tree_root; // Pointer to the GenDat source descriptor tree
};

#endif

