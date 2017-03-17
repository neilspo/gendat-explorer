<?php

/**
* @file
*
* @brief Database search with wildcards
*
* @date 16 March 2017
*
*/



/**
*
* @brief Provides database search with wildcards
*
* This class handles everything required to build, execute, and extract data from complex
* search queries for MySQL databases. The search patterns can contain any number of
* wildcards (* or %) anywhere in the patterns.
*
* To eliminate the possibility of SQL injection attacks, this class only communicates
* with the database using SQL variable binding with prepared statements.
*
*/

class w_search
{
	private $num_args = 0;
	private $arg_list;
	private $where_clause;
	
	private $stmt;
	private $num_rows;
	private $num_matches;
	private $data_row;
	
	
	
	/**
	*
	* @brief Add a field to the WHERE clause
	*
	* @param[in]      $db_field_name    field name in the database
	* @param[in]      $search_string    string to be used as the search pattern
	*
	*/
	
	public function add_field ($db_field_name, $search_string)
	{
		if (!empty($search_string))
		{
			if (++$this->num_args == 1)
				$this->where_clause = ' WHERE ';
			else
				$this->where_clause .= ' AND ';
			
			if (preg_match ('/[*%]/', $search_string))
			{
				$this->where_clause .= $db_field_name . ' LIKE ?';
				$this->arg_list[$this->num_args] = str_replace ('*', '%', $search_string);
			}
			else
			{
				$this->where_clause .= $db_field_name . ' = ?';
				$this->arg_list[$this->num_args] = &$search_string;
			}
		}
	}
	
	
	
	/**
	*
	* @brief Build and execute the search query
	*
	* @param [in,out] $db           MySQLi database object
	* @param [in]     $base_query   base SQL query for the search
	* @param [in]     $max_rows     maximum number of rows that will be obtained from the database (optional)
	*
	*/
	
	public function execute_search ($db, $base_query, $max_rows=0)
	{
		// If $this->stmt already has a value, then assume that it has already been assigned
		// to a prepared statement, which would need to be closed first.
		
		if (isset($this->stmt))
			die ('Need to call w_search::close() first');

		// Create the template SQL query for the prepared statement.
		
		$query = $base_query . $this->where_clause;
		if ($max_rows > 0)
			$query = $query . ' LIMIT ' . $max_rows;
		
		// Have the database server prepare the statement.
		
		if (!($this->stmt=$db->prepare($query)))
			die($db->error);

		// Bind the variables.
		
		$types          = '';
		$param_array[0] = &$types;
		for ($i=1; $i<=$this->num_args; $i++)
		{
			$types           = $types . 's';
			$param_array[$i] = &$this->arg_list[$i];
		}
		if (!call_user_func_array(array($this->stmt, "bind_param"), $param_array))
			die ('Bind failed');
		
		// Execute the prepare statement.
		
		if (!$this->stmt->execute())
			die ("Execute failed");
		
		// Get the number of rows returned by the SQL query.
		// Note: The call to store_result() is only needed here to get the correct number of rows.
		
		$this->stmt->store_result();
		$this->num_rows = $this->stmt->num_rows;
		
		// If the database query returned the maximum allowed number of rows, then ask
		// the MySQL server for the total (unlimited) number of matches.
		
		if ($this->num_rows == $max_rows)
		{
			if (!($result = $db->query("SELECT FOUND_ROWS()"))) die($db->error);
			$temp_array  = $result->fetch_row();
			$this->num_matches = $temp_array[0];
		}
		else
		{
			$this->num_matches = $this->num_rows;
		}
		
		// Get the result set metadata from the prepared statement
		
		$metadata = $this->stmt->result_metadata();
		
		/* Get field information for all columns */
	
		$result_vars = array();
		while ($field_info = $metadata->fetch_field()) {
			$this->data_row[$field_info->name] = NULL;
			$result_vars[] = &$this->data_row[$field_info->name];
		}
		
		if (!call_user_func_array(array($this->stmt, 'bind_result'), $result_vars))
			die ('Bind failed');
	}
	
	
	
	/**
	*
	* @brief Get one data row from the result set of a query
	*
	* @return Either an array containing the data row or the value `FALSE` if there is no more data
	*
	* @note The return array will contain one element for each field in the data row.
	* The \a keys for the array elements will be the field names that were requested in the SQL query,
	* and the corresponding array \a values will be the field data values. The following code fragment
	* shows a typical example of use:
	*
	* @code
	* while ($return_array = $search_obj->fetch_row()) {
	*     $field1_value = $return_array['field1_name'];
	*     $field2_value = $return_array['field2_name'];
	*     ...
	* }
	* @endcode
	*
	*/
	
	public function fetch_row ()
	{
		if ($this->stmt->fetch())
			return $this->data_row;
		else
			return FALSE;
	}
	
	
	
	/**
	*
	* @brief Get the number of data rows obtained from the MySQL server
	*
	* @return    Number of data rows
	*
	*/
	
	public function num_rows ()
	{
		return $this->num_rows;
	}
	
	
	
	/**
	*
	* @brief Get the total (unlimited) number of matches in the database
	*
	* @return    Number of matches
	*
	*/
	
	public function num_matches ()
	{
		return $this->num_matches;
	}
	
	
	/**
	*
	* @brief Close the prepared statement
	*
	*/
	
	public function close ()
	{
		$this->stmt->close();
		$this->stmt        = NULL;
		$this->num_rows    = NULL;
		$this->num_matches = NULL;
		$this->data_row    = NULL;
	}
}
?>