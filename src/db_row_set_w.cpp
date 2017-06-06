///
/// \class db_row_set_w db_row_set_w.h
/// \brief Provides writable row sets.
///
/// This class allows data elements in a row set to be altered and writes any
/// changes back into the database.
///


#include <string>
#include <regex>
#include <vector>
#include <map>
#include "db_row_set_w.h"


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Lock column
///
/// This member function locks a column to prevent changes.
///
/// \param[in] col column number
///
/// \exception std::out_of_range thrown if the column number is out of range
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::lock(unsigned int col)
{
    if (!(col < num_cols()))
        throw std::out_of_range("Bad column number in db_row_set_w::lock");

    col_locks[col] = DB_LOCKED;
}



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Lock column permanently
///
/// This member function permanently locks a column to prevent changes.
/// That column cannot subsequently be unlocked.
///
/// \param[in] col column number
///
/// \exception std::out_of_range thrown if the column number is out of range
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::lock_perm(unsigned int col)
{
    if (!(col < num_cols()))
        throw std::out_of_range("Bad column number in db_row_set_w::lock_perm");

    col_locks[col] = DB_LOCKED_PERM;
}



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Unlock column
///
/// This member function unlocks a column of data to allow changes,
/// which will eventually be written back to the database.
/// Any column that has been permanently locked cannot be unlocked.
///
/// \param[in] col column number
///
/// \return `true` if the column was successfully unlocked
///
/// \exception std::out_of_range thrown if the column number is out of range
///
////////////////////////////////////////////////////////////////////////////////

bool db_row_set_w::unlock(unsigned int col)
{
    if (!(col < num_cols()))
        throw std::out_of_range("Bad column number in db_row_set_w::unlock");

    if (col_locks[col] != DB_LOCKED_PERM)
    {
        col_locks[col] = DB_UNLOCKED;
        return true;
    }
    else
    {
        return false;
    }
}




////////////////////////////////////////////////////////////////////////////////
///
/// \brief Save data
///
/// This member function validates a new value for the data field and prepares
/// it for eventual transmission to the database server.
///
/// \param[in]  row         row number
/// \param[in]  col         column number
/// \param[in]  data        new value for the data field
/// \param[out] error_msg   error message
///
///
/// \return `true` if successful
///
/// \exception std::out_of_range thrown if the row or column number is out of range
///
////////////////////////////////////////////////////////////////////////////////

bool db_row_set_w::save_data(unsigned int row, unsigned int col, std::string data, std::string& error_msg)
{
    std::regex regex_int("^[+-]?[0-9]+$");
    std::regex regex_uint("^[+]?[0-9]+$");
    std::regex regex_float("^[+-]?(([0-9]+[.]?[0-9]*)|([.]?[0-9]+))([eE][+-]?[0-9]+)?$");
    std::regex regex_date("^[0-9]{4}-(0[1-9]|1[012])-(0[1-9]|[12][0-9]|3[01])$");

    // Handle NULL substitution mode.

    if (null_subst_mode & data.empty())
    {
        bool val = save_null (row, col, error_msg);
        return val;
    }

    // Check for valid row and column numbers.

    if (!((row < num_rows()) & (col < num_cols())))
        throw std::logic_error("Row or column number out of range in db_row_set_w::save_data");

    // Make sure this row set is writable

    if (!row_set_is_writable)
    {
        error_msg = "This row set cannot be modified";
        return false;
    }

    // Make sure this column in the row set is not locked.

    if (col_locks[col] != DB_UNLOCKED)
    {
        error_msg = "This data column is locked";
        return false;
    }

    // Validate the data using the appropriate regular expression for that data type.

    switch (col_desc(col)->type())
    {
    case DB_TINYINT:
    case DB_SMALLINT:
    case DB_MEDIUMINT:
    case DB_INT:
    case DB_BIGINT:
        if (!std::regex_match(data, regex_int))
        {
            error_msg = "Data value not integer";
            return false;
        }
        break;
    case DB_UNSIGNED_TINYINT:
    case DB_UNSIGNED_SMALLINT:
    case DB_UNSIGNED_MEDIUMINT:
    case DB_UNSIGNED_INT:
    case DB_UNSIGNED_BIGINT:
        if (!std::regex_match(data, regex_uint))
        {
            error_msg = "Data value not unsigned integer";
            return false;
        }
        break;
    case DB_DECIMAL:
    case DB_FLOAT:
    case DB_DOUBLE:
        if (!std::regex_match(data, regex_float))
        {
            error_msg = "Data value not floating point";
            return false;
        }
        break;
    case DB_DATE:
        if (!std::regex_match(data, regex_date))
        {
            error_msg = "Data value is not a valid date";
            return false;
        }
        break;
    default:
        break;
    }

    // Find the required row map element in the map of altered rows.

    row_map_element *rme = find_row_map_element(row);


    // Make sure this row has not already been marked for deletion.

    if (rme->row_state == DB_DELETE)
    {
        error_msg = "The row was previously marked for deletion";
        return false;
    }

    // Save the new data value.

    rme->row_state = DB_UPDATE;
    rme->field_needs_update[col] = true;
    rme->field_is_null     [col] = false;
    rme->field_value       [col] = data;

    // Show success.

    error_msg.clear();
    return true;
}



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Save NULL
///
/// This member function sets the data field to SQL NULL and prepares
/// it for eventual transmission to the database server.
///
/// \param[in]  row         row number
/// \param[in]  col         column number
/// \param[out] error_msg   error message
///
/// \return `true` if successful, `false` if this field cannot be NULL
///
/// \exception std::out_of_range thrown if the row or column number is out of range
///
////////////////////////////////////////////////////////////////////////////////

bool db_row_set_w::save_null(unsigned int row, unsigned int col, std::string& error_msg)
{

    // Check for valid row and column numbers.

    if (!((row < num_rows()) & (col < num_cols())))
        throw std::logic_error("Row or column number out of range in db_row_set_w::save_null");

    // Make sure this row set is writable

    if (!row_set_is_writable)
    {
        error_msg = "This row set cannot be modified";
        return false;
    }

    // Make sure this column in the row set is not locked.

    if (col_locks[col] != DB_UNLOCKED)
    {
        error_msg = "This data column is locked";
        return false;
    }

    // Make sure the database will allow this column can be set to NULL.

    if (!(col_desc(col)->null_ok()))
    {
        error_msg = "This data column can contain nulls";
        return false;
    }

    // Find the required row map element in the map of altered rows.

    row_map_element *rme = find_row_map_element(row);

    // Make sure this row has not already been marked for deletion.

    if (rme->row_state == DB_DELETE)
    {
        error_msg = "The row was previously marked for deletion";
        return false;
    }

    // Save the new data value.

    rme->row_state = DB_UPDATE;
    rme->field_needs_update[col] = true;
    rme->field_is_null     [col] = true;
    rme->field_value       [col] = "";

    // Show success.

    error_msg.clear();
    return true;
}


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Write cached data to database
///
/// This member function attempts to make all of the changes to the database
/// that are currently held in the cache.
///
/// \param[in]  db         database connection, which must currently be open
///
/// \exception std::runtime_error thrown if the database server reports an error
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::write_to_db(database& db)
{
    for (auto i = altered_rows.begin(); i != altered_rows.end(); i++)
    {
        switch (i->second.row_state)
        {
        case DB_UPDATE:
            update_altered_row(i->first, &i->second, db);
            break;
        case DB_DELETE:
            break;
        case DB_INSERT:
            break;
        default:
            break;
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Turn NULL substitution mode on
///
/// This member function alters the behavior of the `db_row_set_w::save_data`
/// function. Any subsequent attempts to set a database field to an empty
/// string (or the value "") will result in that field being set to SQL NULL.
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::set_null_subst_on()
{
    null_subst_mode = true;
}


////////////////////////////////////////////////////////////////////////////////
///
/// \brief Turn NULL substitution mode off
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::set_null_subst_off()
{
    null_subst_mode = false;
}



////////////////////////////////////////////////////////////////////////////////
///
/// \brief Initialize, phase 1
///
/// This overrides a private virtual function in the base class. It will be
/// executed before the data set returned from a database query is loaded into
/// the base class.
///
/// Delete any existing data in the writable part of the row set and mark
/// it as non-writable.
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::setup_child_phase_1()
{
    // Clear the list of column locks.

    col_locks.clear();

    // Clear the list of database tables.

    my_tables.clear();

    // Clear the map of altered rows.

    altered_rows.clear();

    // Assume for now that the row set that is about to be loaded will be not
    // be writable.

    row_set_is_writable = false;
}




////////////////////////////////////////////////////////////////////////////////
///
/// \brief Initialize, phase 2.
///
/// This overrides a private virtual function in the base class. It will be
/// executed after the data set returned from a database query is loaded into
/// the base class.
///
/// If the database query returned a non-empty row set, then initialize the
/// data structures.
///
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::setup_child_phase_2()
{
    if ((num_rows() > 0) & (num_cols() > 0))
    {
        // Resize the list of column locks, with default state unlocked.

        col_locks.resize(num_cols(), DB_UNLOCKED);

        // Collect information about database tables, columns and primary keys.

        init_db_info();

        // If a table has no primary key, then make that table and all of its
        // columns non-writeable.

        row_set_is_writable = false;
        for (unsigned int i = 0; i < my_tables.size(); i++)
        {
            if (my_tables[i].keys.size() == 0)
            {
                my_tables[i].is_writable = false;
                for (unsigned int j = 0; j < my_tables[i].cols.size(); j++)
                    col_locks[my_tables[i].cols[j]] = DB_LOCKED_PERM;
            }
            else
            {
                my_tables[i].is_writable = true;
                row_set_is_writable      = true;
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
//
// Collect information about database tables, columns and primary keys that are
// referenced in the row set.
//
////////////////////////////////////////////////////////////////////////////////

void db_row_set_w::init_db_info()
{
    for (unsigned int col = 0; col < num_cols(); col++)
    {
        db_col_desc const *col_desc_p = col_desc(col);

        // Both the database column name and the table name will be needed to
        // write any changes to this column back into the database. If either
        // has not been set, then make the column non-writable.

        if (col_desc_p->name_in_db().empty() | col_desc_p->table().empty())
        {
            col_locks[col] = DB_LOCKED_PERM;
        }
        else
        {
            // Find this table in list of tables referenced in the row set.
            // Add it, if it's not already there.

            int i_table = -1;
            for (unsigned int i = 0; i < my_tables.size(); i++)
            {
                if (col_desc_p->table() == my_tables[i].name)
                {
                    i_table = i;
                    break;
                }
            }
            if (i_table == -1)
            {
                my_tables.push_back({ col_desc_p->table(), false });
                i_table = my_tables.size() - 1;
            }

            // Add this column number to the list of columns referenced from this table.

            my_tables[i_table].cols.push_back(col);

            // If this column is part of a primary key, then add it to the list of
            // primary keys for the table.

            if (col_desc_p->is_pri_key())
            {
                my_tables[i_table].keys.push_back(col);

                // If the key is set to auto-increment, then let the database server set the value.

                if (col_desc_p->is_auto_inc())
                {
                    col_locks[col] = DB_LOCKED_PERM;
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Create and execute one UPDATE query for each table referenced in the row set.
//
////////////////////////////////////////////////////////////////////////////////


void db_row_set_w::update_altered_row(unsigned int row, row_map_element *p_alt_row, database& db)
{
    std::string query;

    // Construct an UPDATE query for each database table that has one or more
    // altered fields.

    for (unsigned int i = 0; i < my_tables.size(); i++)
    {
        table_info *p_table = &my_tables[i];

        if (p_table->is_writable)
        {
            // Find any fields that belong to this table, which need to be updated.

            int altered_fields = 0;
            for (unsigned int j = 0; j < p_table->cols.size(); j++)
            {
                unsigned int col = p_table->cols[j];

                // If this field needs to be updated, then add it to the UPDATE query.

                if (p_alt_row->field_needs_update[col])
                {
                    if (altered_fields++ == 0)
                    {
                        query = "UPDATE " + p_table->name + " SET ";
                    }
                    else
                    {
                        query += ", ";
                    }
                    query += col_desc(col)->name_in_db() + " = ";
                    if (p_alt_row->field_is_null[col])
                    {
                        query += "NULL";
                    }
                    else
                    {
                        query += "\'" + db.escape_str(p_alt_row->field_value[col]) + "\'";
                    }
                }
            }

            // Add the WHERE clause to the query.

            if (altered_fields)
            {
                for (unsigned int j = 0; j < p_table->keys.size(); j++)
                {
                    unsigned int col = p_table->keys[j];

                    std::string key;
                    db_row_set::get_data(row, col, key);

                    // Add this column in the primary key to the query.

                    if (j == 0)
                    {
                        query += " WHERE ";
                    }
                    else
                    {
                        query += " AND ";
                    }
                    query += col_desc(col)->name_in_db() + " = \'" + db.escape_str(key) + "\'";
                }

                // Execute the query.

                unsigned int affected_rows = db.execute(query);

                // If the query succeeded, then mark the fields as not needing update.

                if (affected_rows)
                {
                    for (unsigned int j = 0; j < p_table->cols.size(); j++)
                    {
                        p_alt_row->field_needs_update[p_table->cols[j]] = false;
                    }
                }
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// Find this row in the list of altered rows. If it's not already in the
// list, then add it.
//
///////////////////////////////////////////////////////////////////////////////

db_row_set_w::row_map_element * db_row_set_w::find_row_map_element(unsigned int row)
{
    auto irow = altered_rows.find(row);
    row_map_element *rme;
    if (irow == altered_rows.end())
    {
        rme = &altered_rows[row];
        rme->field_needs_update.resize (num_cols(), false);
        rme->field_is_null.resize      (num_cols());
        rme->field_value.resize        (num_cols());
    }
    else
    {
        rme = &irow->second;
    }

    return rme;
}


