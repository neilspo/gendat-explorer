#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <mysql.h>
#include "db_row_set.h"

class database
{
public:
	database();
	~database();

	void         connect    (std::string host, std::string user, std::string passwd, std::string db_name);
	void         disconnect ();
	void         execute	(std::string query,
							 std::vector <std::vector <std::string>>& result_set,
							 unsigned int& num_rows,
							 unsigned int& num_cols);
	void         execute    (std::string query, db_row_set& row_set);
	unsigned int execute    (std::string query);
	std::string  escape_str (std::string str);
private:
	void execute_1(
		std::string query,
		std::vector <std::vector <std::string>>& result_set,
		std::vector <std::vector<bool>> *null_fields_prt,
		unsigned int& num_rows,
		unsigned int& num_cols,
		MYSQL_RES *&result);

	MYSQL *db_connection;
	bool   db_connected;
};

#endif
