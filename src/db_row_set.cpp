///
/// \class db_row_set db_row_set.h
/// \brief Handles sets of data from the database.
///
/// This class encapsulates the data set returned from a database query, along with
/// metadata needed to use those data. All of the data and metadata held in this class were
/// obtained from the database server and cannot be changed.
///


#include <stdexcept>
#include "db_row_set.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of rows in the row set.
///
/// \return number of rows
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int db_row_set::num_rows() const
{
	return my_num_rows;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the number of columns in the row set.
///
/// \return number of columns
///
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int db_row_set::num_cols() const
{
	return my_num_cols;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get column name
///
/// \param[in]  col   column number
///
/// \return name of the column
///
/// \exception std::out_of_range thrown if the column number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string db_row_set::col_name(unsigned int col) const
{
	if (col < my_num_cols)
		return col_desc_list[col].name();
	else
		throw std::out_of_range("Bad column number in db_row_set::col_name");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get the contents of one data field
///
/// \param[in]  row   row number
/// \param[in]  col   column number
/// \param[out] data  contents of the data field
///
/// \return     True if the data field was set. False if the data field was null (empty).
///
/// \exception std::out_of_range thrown if the row or column number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool db_row_set::get_data(unsigned int row, unsigned int col, std::string& data) const
{
	if ((row >= my_num_rows) || (col >= my_num_cols))
		throw std::logic_error("Row or column number out of range in db_row_set::get_data");

	if (null_fields[row][col])
	{
		data.clear();
		return false;
	}
	else
	{
		data = result_set[row][col];
		return true;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Get column descriptor
///
/// This member function provides readonly access to the specified column descriptor.
///
/// \param[in]  col   column number
///
/// \return pointer to the column descriptor
///
/// \exception std::out_of_range thrown if the column number is out of range
///
////////////////////////////////////////////////////////////////////////////////////////////////////
db_col_desc const * db_row_set::col_desc(unsigned col) const
{
    if (col < my_num_cols)
        return &col_desc_list[col];
    else
        throw std::out_of_range("Bad column number in db_row_set::col_desc");
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Clear the row_set
///
/// This member function clears all data and meta data in the row set.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void db_row_set::clear()
{
	my_num_rows = 0;
	my_num_cols = 0;

	// Clear the column descriptors.

	col_desc_list.clear();

	// Clear the map of null fields.

	null_fields.clear();

	// Clear the data.

	result_set.clear();
}
