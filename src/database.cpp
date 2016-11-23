///
/// \class database database.h
///
/// \brief Provides communications with the database server.
///
/// This class handles all low-level communications to and from the actual MySQL database server.
/// Any errors received from the server will generate a C++ exception, which will contain a message
/// that describes the problem.
/// 


#include <string>
#include <cstring>
#include <stdexcept>
#include "database.h"


/*

Constructor

*/

database::database()
{
	db_connected = false;
}


/*

Destructor

*/

database::~database()
{
	if (db_connected)
	{
		disconnect();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Connect to database
///
/// This member function allocates the required data structures and attempts to connect to the database.
///
/// \param[in] host     database host name or IP address
/// \param[in] user     database user login ID
/// \param[in] passwd   database user password
/// \param[in] db_name  database name
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void database::connect(std::string host, std::string user, std::string passwd, std::string db_name)
{
	if (db_connected)
		throw std::runtime_error("Database already connected");

	db_connection = mysql_init(NULL);
	if (db_connection == NULL)
		throw std::runtime_error("Insufficient memory to allocate database connector");

	if (NULL == mysql_real_connect(db_connection, host.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), 0, NULL, 0))
		throw std::runtime_error(mysql_error(db_connection));

	db_connected = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Disconnect from database
///
/// This member function disconnects from the database and deallocates any data structures associated
/// with the connection.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void database::disconnect()
{
	if (db_connected)
	{
		mysql_close(db_connection);
		db_connected = false;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Execute database query
///
/// This member function transfers one STL query to the database server for execution and obtains
/// the result set (if one was produced).
///
/// If the query returned no results, then `num_cols` is set to zero. 
/// Non-SELECT queries that alter the database (such as INSERT, UPDATE, or DELETE) never
/// produce a result set. For these types of queries, `num_rows` is set to the number of rows
/// that were changed (affected) in the database.
///
/// If `num_cols` is non-zero, then the query produced a result set, which contained `num_cols`
/// columns and `num_rows` rows of data. If `num_rows` is zero, then the result set was empty. This
/// most often happens when a SELECT query, with a WHERE clause, matched no rows in the database.
///
///
/// \param[in]  query        a single SQL statement (without a terminating semicolon)
/// \param[out] result_set   a two-dimensional dynamic array with the query result set.
/// \param[out] num_rows     number of rows in the result set
/// \param[out] num_cols     number of columns in the result set
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////


void database::execute(std::string query, std::vector <std::vector <std::string>>& result_set, unsigned int& num_rows, unsigned int& num_cols)
{
	MYSQL_RES *mysql_result = nullptr;

	// Assume the query returns nothing.

	num_rows = 0;
	num_cols = 0;
	result_set.clear();

	// Do the work.

	execute_1 (query, result_set, nullptr, num_rows, num_cols, mysql_result);

	// This version of this member function does not require any more information.
	// Clean up.

	if (mysql_result != nullptr) mysql_free_result(mysql_result);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Execute database query
///
/// Use this version to execute a query that should return results (such as SELECT or DESCRIBE).
///
/// \param[in]  query     a single SQL statement (without a terminating semicolon)
/// \param[out] row_set   results from the query
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void database::execute(std::string query, db_row_set& row_set)
{
	MYSQL_RES   *mysql_result;
	MYSQL_FIELD *mysql_fields;

	// Prepare the base class for the database query.

	row_set.clear();
	
	// Prepare a child class for the query.

	row_set.setup_child_phase_1();

	// Execute the query.

	execute_1(query, row_set.result_set, &row_set.null_fields, row_set.my_num_rows, row_set.my_num_cols, mysql_result);

	// Resize the column descriptor list.

	row_set.col_desc_list.resize(row_set.my_num_cols);

	// Copy the required information from the MySQL result-set into the column descriptions.

	mysql_fields = mysql_fetch_fields(mysql_result);
	for (unsigned int i = 0; i < row_set.my_num_cols; i++)
	{
		// Column name

		if (mysql_fields[i].name != NULL)
			row_set.col_desc_list[i].my_name = mysql_fields[i].name;

		// Column name in the database (aliases are ignored)

		if (mysql_fields[i].org_name != NULL)
			row_set.col_desc_list[i].my_name_in_db = mysql_fields[i].org_name;

		// Database table to which the column belongs

		if (mysql_fields[i].table != NULL)
			row_set.col_desc_list[i].my_table = mysql_fields[i].table;

		// Data length

		row_set.col_desc_list[i].my_length = mysql_fields[i].length;

		// Number of decimals for numeric data fields

		row_set.col_desc_list[i].my_decimals = mysql_fields[i].decimals;

		// Determine if nulls are allowed

		row_set.col_desc_list[i].my_null_ok = (0 == (mysql_fields[i].flags & NOT_NULL_FLAG));

		// Determine if this column is part of a primary key

		row_set.col_desc_list[i].my_pri_key = (0 != (mysql_fields[i].flags & PRI_KEY_FLAG));

		// Determine if this column auto-increments

		row_set.col_desc_list[i].my_auto_inc = (0 != (mysql_fields[i].flags & AUTO_INCREMENT_FLAG));
		
		// Data type

		switch (mysql_fields[i].type)
		{
		case MYSQL_TYPE_TINY:
			if (mysql_fields[i].flags & UNSIGNED_FLAG)
				row_set.col_desc_list[i].my_type = DB_UNSIGNED_TINYINT;
			else
				row_set.col_desc_list[i].my_type = DB_TINYINT;
			break;
		case MYSQL_TYPE_SHORT:
			if (mysql_fields[i].flags & UNSIGNED_FLAG)
				row_set.col_desc_list[i].my_type = DB_UNSIGNED_SMALLINT;
			else
				row_set.col_desc_list[i].my_type = DB_SMALLINT;
			break;
		case MYSQL_TYPE_INT24:
			if (mysql_fields[i].flags & UNSIGNED_FLAG)
				row_set.col_desc_list[i].my_type = DB_UNSIGNED_MEDIUMINT;
			else
				row_set.col_desc_list[i].my_type = DB_MEDIUMINT;
			break;
		case MYSQL_TYPE_LONG:
			if (mysql_fields[i].flags & UNSIGNED_FLAG)
				row_set.col_desc_list[i].my_type = DB_UNSIGNED_INT;
			else
				row_set.col_desc_list[i].my_type = DB_INT;
			break;
		case MYSQL_TYPE_LONGLONG:
			if (mysql_fields[i].flags & UNSIGNED_FLAG)
				row_set.col_desc_list[i].my_type = DB_UNSIGNED_BIGINT;
			else
				row_set.col_desc_list[i].my_type = DB_BIGINT;
			break;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			row_set.col_desc_list[i].my_type = DB_DECIMAL;
			break;
		case MYSQL_TYPE_FLOAT:
			row_set.col_desc_list[i].my_type = DB_FLOAT;
			break;
		case MYSQL_TYPE_DOUBLE:
			row_set.col_desc_list[i].my_type = DB_DOUBLE;
			break;
		case MYSQL_TYPE_BIT:
			row_set.col_desc_list[i].my_type = DB_BIT;
			break;
		case MYSQL_TYPE_TIMESTAMP:
			row_set.col_desc_list[i].my_type = DB_TIMESTAMP;
			break;
		case MYSQL_TYPE_DATE:
			row_set.col_desc_list[i].my_type = DB_DATE;
			break;
		case MYSQL_TYPE_TIME:
			row_set.col_desc_list[i].my_type = DB_TIME;
			break;
		case MYSQL_TYPE_DATETIME:
			row_set.col_desc_list[i].my_type = DB_DATETIME;
			break;
		case MYSQL_TYPE_YEAR:
			row_set.col_desc_list[i].my_type = DB_YEAR;
			break;
		case MYSQL_TYPE_STRING:
			if (mysql_fields[i].charsetnr == 63)
				row_set.col_desc_list[i].my_type = DB_BINARY;
			else
				row_set.col_desc_list[i].my_type = DB_CHAR;
			break;
		case MYSQL_TYPE_VAR_STRING:
			if (mysql_fields[i].charsetnr == 63)
				row_set.col_desc_list[i].my_type = DB_VARBINARY;
			else
			row_set.col_desc_list[i].my_type = DB_VARCHAR;
			break;
		case MYSQL_TYPE_BLOB:
			if (mysql_fields[i].charsetnr == 63)
				row_set.col_desc_list[i].my_type = DB_BLOB;
			else
				row_set.col_desc_list[i].my_type = DB_TEXT;
			break;
		case MYSQL_TYPE_SET:
			row_set.col_desc_list[i].my_type = DB_SET;
			break;
		case MYSQL_TYPE_ENUM:
			row_set.col_desc_list[i].my_type = DB_ENUM;
			break;
		case MYSQL_TYPE_GEOMETRY:
			row_set.col_desc_list[i].my_type = DB_GEOMETRY;
			break;
		case MYSQL_TYPE_NULL:
			row_set.col_desc_list[i].my_type = DB_NULL;
			break;
		default:
			row_set.col_desc_list[i].my_type = DB_UNKNOWN_TYPE;
		}
	}

	// Clean up the MySQL data structures.

	mysql_free_result(mysql_result);

	// Allow a child class to prepare its data structures using the query results.

	row_set.setup_child_phase_2();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Execute database query
///
/// Use this version to execute a query (such as INSERT, UPDATE, or DELETE) that will alter the
/// database, but not return a result set.
///
/// \param[in]  query a single SQL statement (without a terminating semicolon)
///
/// \return   number of affected rows in the database
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int database::execute(std::string query)
{
	// Allocate results_set, in case this member function is incorrectly called with a query that
	// will return results.  

	std::vector <std::vector <std::string>> result_set;
	unsigned int num_rows;
	unsigned int num_cols;

	// Execute the query.

	execute(query, result_set, num_rows, num_cols);

	// If num_cols is zero, then this was a query that altered the database and
	// we should return the number of affected rows.

	if (num_cols == 0)
	{
		return num_rows;
	}
	else
	{
		return 0;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Escape string
///
/// This member function encodes special characters in the input string to produce an escaped string
/// that is safe to use in an SQL statement, taking into account the current character set of the
/// database connection.
///
/// \param[in] str  string to be processed
///
/// \return  escaped string
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string database::escape_str(std::string str)
{
	char* esc_str = new char[strlen(str.c_str()) * 2 + 1];
	unsigned int length = mysql_real_escape_string(db_connection, esc_str, str.c_str(), str.length());
	std::string result(esc_str, length);
	delete esc_str;
	return result;
}



// This private member function does the first part of the various versions of the public "execute" function.

void database::execute_1(
			std::string query,
			std::vector <std::vector <std::string>>& result_set,
			std::vector <std::vector<bool>> *null_fields_ptr,
			unsigned int& num_rows,
			unsigned int& num_cols,
			MYSQL_RES *&result)
{
	MYSQL_ROW row;

	// We need to be connected to a database to continue.

	if (!db_connected)
		throw std::runtime_error("No database connection");

	// Execute the SQL query.

	if (mysql_query(db_connection, query.c_str()) != 0)
		throw std::runtime_error(mysql_error(db_connection));

	// If the query produced a result set, then num_cols will be non-zero.

	num_cols = mysql_field_count(db_connection);

	if (num_cols == 0)
	{
		// No result set was sent from the database server, but that is what should happen for
		// most non-SELECT queries that alter the database (such as INSERT, UPDATE, DELETE).
		// In this case, set num_rows to the number of rows that were changed (affected).

		num_rows = (unsigned int) mysql_affected_rows(db_connection);
	}
	else
	{
		// Get the result set from the database server.

		result = mysql_store_result(db_connection);
		if (result == NULL)
			throw std::runtime_error(mysql_error(db_connection));

		// Get the number of rows in the result set.

		num_rows = (int)mysql_num_rows(result);

		// If there are no rows in the result set, then just return.

		if (num_rows == 0)
			return;

		// Resize the data arrays to hold the correct number of rows.

		if (null_fields_ptr != nullptr) (*null_fields_ptr).resize(num_rows);
		result_set.resize(num_rows);

		// Copy the data in the result set to where it should go.

		for (unsigned int i = 0; i < num_rows; i++)
		{
			// Get one row of data from the result set.

			row = mysql_fetch_row(result);
			if (row == NULL)
				throw std::runtime_error(mysql_error(db_connection));			

			// Resize the data arrays to hold the correct number of columns.

			if (null_fields_ptr != nullptr) (*null_fields_ptr)[i].resize(num_cols);
			result_set[i].resize(num_cols);
			
			// Copy the data to the final location.

			for (unsigned int j = 0; j < num_cols; j++)
			{
				if (row[j] == nullptr)
				{
					if (null_fields_ptr != nullptr) (*null_fields_ptr)[i][j] = true;
				}
				else
				{
					if (null_fields_ptr != nullptr) (*null_fields_ptr)[i][j] = false;
					result_set[i][j] = row[j];
				}
			}
		}
	}
}
