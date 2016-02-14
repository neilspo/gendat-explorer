<?php

/**
* @file
*
* @brief Connect to a PgpGedView database.
* @date 13 February 2016
*
*/


// ******************************************************************
// I need to change the function names pgv_indi_url, pgv_individual
// and pgv_ind to something that is less confusing.
// ******************************************************************


include_once ('misc_functions.php');
include_once ('config.php');

/**
*
* Generate the URL for an PGV individual.
*
*/

function pgv_indi_url($n_id)
{
	global $pgv_path;
	if ($n_id == null)
	{
		return null;
	}
	else
	{
		return $pgv_path . 'individual.php?pid=' . $n_id . '&ged=base.ged';
	}
}




/**
* 
* Generate a link to a PGV individual.
*
*/

function pgv_individual($n_id)
{
	$url = pgv_indi_url($n_id);
	return html_link($n_id,$url);
}

/**
*
* @brief Get the name of an individual.
*
*/

function pgv_get_name($db, $n_id)
{
	global $pgv_prefix;
	$query = "SELECT n_list FROM {$pgv_prefix}pgv_name WHERE n_id='$n_id'";
	$result = $db->query($query);
	if (!$result) die($db->error);
	if ($data = $result->fetch_object())
	{
		$name = $data->n_list;
		
		// PGV uses the string '@N.N.' to represent an unknown name.  Replace that string
		// with '(unknown)'. Return NULL if the individual cannot be found in the database.
		
		$name = str_replace ('@N.N.', '(unknown)', $name);
		return $name;
	}
	else
	{
		return null;
	}
}

class pgv_ind
{
	public $n_id;
	public $name;
	public $link;
	public $birth_date;
	public $birth_place;
	public $death_date;
	public $death_place;
	public $father_id;
	public $father_name;
	public $father_link;
	public $mother_id;
	public $mother_name;
	public $mother_link;
	
	function __construct($db, $n_id)
	{
		global $pgv_prefix;
		$this->n_id = $n_id;
		
		// Load the person's name.
		
		if (($this->name = pgv_get_name($db,$n_id)) != null)
		{
			$this->link = html_link ($this->name, pgv_indi_url($n_id));
		}
		else
		{
			$this->link = $n_id;
			return;
		}
		
		// Load the date of birth.
		
	$query = "SELECT DoB FROM {$pgv_prefix}births_1 WHERE i_id='$n_id'";
		$result = $db->query($query);
		if ($data = $result->fetch_object())
		{
			$this->birth_date = $data->DoB;
		}
		else
		{
			$this->birth_date = null;
		}
		
		// Load the place of birth.
		
		$query = "SELECT PoB FROM {$pgv_prefix}births_2 WHERE i_id='$n_id'";
		$result = $db->query($query);
		if ($data = $result->fetch_object())
		{
			$this->birth_place = $data->PoB;
		}
		else
		{
			$this->birth_place = null;
		}
		
		// Load the date of death.
		
		$query = "SELECT DoD FROM {$pgv_prefix}deaths_1 WHERE i_id='$n_id'";
		$result = $db->query($query);
		if ($data = $result->fetch_object())
		{
			$this->death_date = $data->DoD;
		}
		else
		{
			$this->death_date = null;
		}
		
		// Load the place of death.
		
		$query = "SELECT PoD FROM {$pgv_prefix}deaths_2 WHERE i_id='$n_id'";
		$result = $db->query($query);
		if ($data = $result->fetch_object())
		{
			$this->death_place = $data->PoD;
		}
		else
		{
			$this->death_place = null;
		}
		
		// Load information about the parents.
		
		$query = "SELECT * FROM {$pgv_prefix}parents WHERE i_id='$n_id'";
		$result = $db->query($query);
		if ($data = $result->fetch_object())
		{
			// Father
			
			$this->father_id = $data->father;
			if (($this->father_name = pgv_get_name($db, $this->father_id)) != null)
			{
				$this->father_link = html_link ($this->father_name, pgv_indi_url($this->father_id));
			}
			else
			{
				$this->father_link = $this->father_id;
			}
			
			// Mother
			
			$this->mother_id = $data->mother;
			if (($this->mother_name = pgv_get_name($db, $this->mother_id)) != null)
			{
				$this->mother_link = html_link ($this->mother_name, pgv_indi_url($this->mother_id));
			}
			else
			{
				$this->mother_link = $this->mother_id;
			}
		}
	}
}
?> 