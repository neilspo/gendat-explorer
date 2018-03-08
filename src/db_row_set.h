///
/// \file db_row_set.h
///


#ifndef DB_ROW_SET_H
#define DB_ROW_SET_H

#include <string>
#include <vector>

///
/// \brief Data types
///
/// These are the known types of data that could be received from the database server.
/// If the data type could not be determined or was not recognized, then the type will
/// be set to `DB_UNKNOWN_TYPE'.
///

enum db_data_type
{
	DB_UNKNOWN_TYPE = 0,   ///< Unknown data type
	DB_TINYINT,            ///< Integer, 1 byte
	DB_SMALLINT,           ///< Integer, 2 bytes
	DB_MEDIUMINT,          ///< Integer, 3 bytes
	DB_INT,                ///< Integer, 4 bytes
	DB_BIGINT,             ///< Integer, 8 bytes
	DB_UNSIGNED_TINYINT,   ///< Unsigned integer, 1 byte
	DB_UNSIGNED_SMALLINT,  ///< Unsigned integer, 2 bytes
	DB_UNSIGNED_MEDIUMINT, ///< Unsigned integer, 3 bytes
	DB_UNSIGNED_INT,       ///< Unsigned integer, 4 bytes
	DB_UNSIGNED_BIGINT,    ///< Unsigned integer, 8 bytes
	DB_DECIMAL,            ///< Exact value, fixed-point with given precision and scale
	DB_FLOAT,              ///< Approximate value, floating-point with given precision
	DB_DOUBLE,             ///< Approximate value, double precision floating-point
	DB_BIT,                ///< Bit-field with a given number of bits
	DB_TIMESTAMP,          ///< TIMESTAMP data type
	DB_DATE,               ///< DATE data type
	DB_TIME,               ///< TIME data type
	DB_DATETIME,           ///< DATETIME data type
	DB_YEAR,               ///< YEAR data type
	DB_CHAR,               ///< Fixed length character string
	DB_VARCHAR,            ///< Variable length character string
	DB_BINARY,             ///< Fixed length binary string
	DB_VARBINARY,          ///< Variable length binary string
	DB_BLOB,               ///< Binary large object
	DB_TEXT,               ///< Character large object
	DB_SET,                ///< String with zero or more of the permitted values
	DB_ENUM,               ///< String with one of the permitted values
	DB_GEOMETRY,           ///< Spatial data type
	DB_NULL                ///< Null type
};

///
/// \class db_col_desc db_row_set.h
///
/// \brief Database column descriptor
///
/// This class holds static information that describes one database column.
///

class db_col_desc
{
public:
	std::string  name       () const { return my_name;       } ///< Column name
	std::string  name_in_db () const { return my_name_in_db; } ///< Column name in the database
	std::string  table      () const { return my_table;      } ///< Database table to which the column belongs
	db_data_type type       () const { return my_type;       } ///< Data type
	unsigned int length     () const { return my_length;     } ///< Data length (precision)
	unsigned int decimals   () const { return my_decimals;   } ///< Number of decimals (scale) for numeric data types
	bool         null_ok    () const { return my_null_ok;    } ///< True if nulls are allowed in column
	bool         is_pri_key () const { return my_pri_key;    } ///< True if column is part of a primary key
	bool         is_auto_inc() const { return my_auto_inc;   } ///< True if column is set to auto-increment

private:
	std::string  my_name;
	std::string  my_name_in_db;
	std::string  my_table;
	db_data_type my_type     = DB_UNKNOWN_TYPE;
	unsigned int my_length   = 0;
	unsigned int my_decimals = 0;
	bool         my_null_ok  = false;
	bool         my_pri_key  = false;
	bool         my_auto_inc = false;

	friend class database;
};



class db_row_set
{
	friend class database;

public:
	unsigned int  num_rows () const;
	unsigned int  num_cols () const;
	std::string   col_name (unsigned int col) const;
	bool          get_data (unsigned int row, unsigned int col, std::string& data) const;

	db_col_desc const * col_desc(unsigned int col) const;

private:
	virtual void setup_child_phase_1() {};
	virtual void setup_child_phase_2() {};

	void clear();

	unsigned int              my_num_cols = 0;            // Number of columns in this row set
	unsigned int              my_num_rows = 0;            // Number of rows in this row set
	std::vector<db_col_desc>  col_desc_list;              // List of column descriptors

	std::vector <std::vector <bool>>        null_fields;  // True if the data field is NULL
	std::vector <std::vector <std::string>> result_set;   // Result set from database query
};

#endif
