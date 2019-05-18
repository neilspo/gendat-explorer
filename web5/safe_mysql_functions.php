<?php

/**
* @file
*
* @brief Safe MySQL functions.
*
* The following functions perform various database operations, using prepared SQL statements.
* This protects against the possibility of SQL injection of malicious code.
*
* Note that some of these functions use the '...' operator, which only works for PHP version 5.6 and highter.
*
* @date 18 May 2019
*
*/



/**
* @brief Test for valid database key.
*
* This function determines if the database contains a row with the required key value.
*
* @param[in,out] $db           MySQLi database object 
* @param[in]     $table        Table name
* @param[in]     $key_field    Primary key field name
* @param[in]     $key_value    Primary key data value 
*
* @returns       Returns a boolean value.
*
*/

function is_valid_key ($db, $table, $key_field, $key_value)
{
	$query = "SELECT EXISTS(SELECT * FROM `$table` WHERE $key_field=?) AS result";
	
	// Run the query.
	
	$param_values[0] = $key_value;
	$data_values = safe_db_select ($db, $query, $param_values);
	
	return (bool) $data_values[0]['result'];
}


/**
* @brief Get the next valid database key.
*
* This function finds the next greater key value in the database table.
*
* @param[in,out] $db           MySQLi database object 
* @param[in]     $table        Table name
* @param[in]     $key_field    Primary key field name
* @param[in]     $key_value    Primary key data value
*
* @returns       Returns the next key value or `null` if there are none left.
*
*/

function get_next_valid_key ($db, $table, $key_field, $key_value)
{
	$query = "SELECT $key_field FROM $table WHERE $key_field > ? ORDER BY $key_field ASC LIMIT 1";
	
	// Run the query.
	
	$param_values[0] = $key_value;
	$data_values = safe_db_select ($db, $query, $param_values);
	
	// There should be one data row returned.
	
	if (count($data_values) == 1)
		$next_key_value = $data_values[0][$key_field];
	else
		$next_key_value = null;

	return $next_key_value;
}



/**
* @brief Update one MySQL row.
*
* This function provides an easy way to update one existing row in the database.
*
* @param[in,out] $db           MySQLi database object 
* @param[in]     $table        Table name
* @param[in]     $data_fields  Array of field names
* @param[in]     $data_values  Array of field data values
* @param[in]     $key_field    Primary key field name
* @param[in]     $key_value    Primary key data value
*
* @returns       Returns the number of affected rows.
*
*/

function db_update_data ($db, $table, $data_fields, $data_values, $key_field, $key_value)
{
	// --- Create the template SQL query for the prepared statement.

	$query = "UPDATE $table SET $data_fields[0]=?";
	for ($i=1; $i<count($data_fields); $i++)
		$query .= ", $data_fields[$i]=?";
	$query .= " WHERE $key_field=?";
	
	// Run the query.
	
	$param_values = array_merge($data_values, array($key_value));
	$num_affected_rows = safe_db_query_nr ($db, $query, $param_values);
	
	return $num_affected_rows;
}



/**
* @brief Insert one MySQL row.
*
* This function provides an easy way to insert one new row into the database.
*
* @param[in,out] $db           MySQLi database object 
* @param[in]     $table        Table name
* @param[in]     $data_fields  Array of field names
* @param[in]     $data_values  Array of field data values
*
* @returns       Returns the number of affected rows.
*
*/

function db_insert_data ($db, $table, $data_fields, $data_values)
{
	// --- Create the template SQL query for the prepared statement.

	$query = "INSERT INTO $table SET $data_fields[0]=?";
	for ($i=1; $i<count($data_fields); $i++)
		$query .= ", $data_fields[$i]=?";
	
	// Run the query.
	
	$num_affected_rows = safe_db_query_nr ($db, $query, $data_values);
	
	return $num_affected_rows;
}



/**
* @brief Obtain data from the database.
*
* This function provides an easy way to obtain data from one database row.
*
* @param[in,out] $db           MySQLi database object 
* @param[in]     $table        Table name
* @param[in]     $data_fields  Array of field names
* @param[in]     $key_field    Primary key field name
* @param[in]     $key_value    Primary key data value
*
* @returns       Returns an associative array with the resulting data rows, or `null`
*                if the query returned no data.
* @note
* The "key field" is not actually required to be the primary key field for the table. If it is not
* the primary key, then the function obtains data from the row(s) where the value in the
* specified "key field" equals `$key_value`.
*
*/

function db_read_data ($db, $table, $data_fields, $key_field, $key_value)
{
	// Create the template SQL query for the prepared statement.

	$query = "SELECT $data_fields[0]";
	for ($i=1; $i<count($data_fields); $i++)
		$query .= ", $data_fields[$i]";
	$query .= " FROM $table WHERE $key_field=?";
	
	// Run the query.
	
	$param_values[0] = $key_value;
	$data_values = safe_db_select ($db, $query, $param_values);

	return $data_values;
}



/**
* @brief Run a SELECT query.
*
* This function will safely run one SELECT query and obtain the result set.
* 
* @param[in,out] $db            MySQLi database object
* @param[in]     $query         SQL statement template
* @param[in]     $param_values  Array of parameter values to use with the statement template
*
* @returns       Returns an associative array with the resulting data rows, or `null`
*                if the query returned no data.
*
*/

function safe_db_select ($db, $query, $param_values)
{
	// Have the database prepare the statement.
	
	if (!($stmt=$db->prepare($query)))
		die("Prepare failed: (" . $db->errno . ") " . $db->error);

	// Bind the query parameter values.
	
	$types = str_repeat('s', count($param_values));
	if (!$stmt->bind_param($types, ...$param_values))
		die ("Binding parameters failed: (" . $stmt->errno . ") " . $stmt->error);
	
	// --- Execute the prepare statement.
	
	if (!$stmt->execute())
		die("Execute failed: (" . $stmt->errno . ") " . $stmt->error);
		
	// If the query produced a result set, then return it in an associative array.
	// Otherwise, just return null.
	
	if ($result = $stmt->get_result())
		$data_values = $result->fetch_all(MYSQLI_ASSOC);
	else 
		$data_values = null;
	
	// --- Release the prepared statement.
	
	$stmt->close();
	
	return $data_values;
}



/**
* @brief Run a query with no results.
*
* This function should be used to safely run INSERT, UPDATE, or DELETE queries.
* These types of queries modify the database, but do not produce a result set.
*
* @param[in,out] $db            MySQLi database object
* @param[in]     $query         SQL statement template
* @param[in]     $param_values  Array of parameter values to use with the statement template
*
* @returns       Returns the number of affected rows.
*
*/

function safe_db_query_nr ($db, $query, $param_values)
{
	// Have the database prepare the statement.
	
	if (!($stmt=$db->prepare($query)))
		die("Prepare failed: (" . $db->errno . ") " . $db->error);

	// Bind the query parameter values.
	
	$types = str_repeat('s', count($param_values));
	if (!$stmt->bind_param($types, ...$param_values))
		die ("Binding parameters failed: (" . $stmt->errno . ") " . $stmt->error);
	
	// --- Execute the prepare statement.
	
	if (!$stmt->execute()) 
		die("Execute failed: (" . $stmt->errno . ") " . $stmt->error);

	// Get the number of affected rows.
	
	$num_affected_rows = $stmt->affected_rows;
	
	// --- Release the prepared statement.
	
	$stmt->close();
	
	return $num_affected_rows;
}
?> 