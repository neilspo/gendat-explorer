///
/// \file db_row_set_w.h
///

#ifndef DB_ROW_SET_W_H
#define DB_ROW_SET_W_H

#include <string>
#include <vector>
#include <map>
#include "db_row_set.h"
#include "database.h"


class db_row_set_w : public db_row_set
{
public:
	void lock        (unsigned int col);
	void lock_perm   (unsigned int col);
	bool unlock      (unsigned int col);
	bool save_data   (unsigned int row, unsigned int col, std::string data, std::string& error_msg);
	bool save_null   (unsigned int row, unsigned int col);
	void write_to_db (database& db);

private:

	// These override private virtual functions in the base class.

	void setup_child_phase_1();
	void setup_child_phase_2();

	enum db_lock_state
	{
		DB_UNLOCKED = 1,
		DB_LOCKED,
		DB_LOCKED_PERM
	};
	enum db_row_state
	{
		DB_UPDATE = 1,
		DB_INSERT,
		DB_DELETE,
	};

	bool row_set_is_writable = false;


	// List of column locks

	std::vector	<db_lock_state> col_locks;


	// Map of altered rows

	struct row_map_element
	{
		db_row_state               row_state;
		std::vector <bool>         field_needs_update;
		std::vector <bool>         field_is_null;
		std::vector <std::string>  field_value;
	};
	std::map <unsigned int, row_map_element> altered_rows;


	// Information about database tables referenced in this row set.

	struct table_info
	{
		std::string                name;
		bool                       is_writable;
		std::vector <unsigned int> keys;
		std::vector <unsigned int> cols;
	};
	std::vector <table_info> my_tables;


	void init_db_info();
	void update_altered_row(unsigned int alt_row, row_map_element* p_alt_row, database& db);
};

#endif