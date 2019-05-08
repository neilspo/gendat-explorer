<?php

/**
* @file
*
* @brief Connect to a PhpGedView database.
*
* @date 13 October 2016
*
*/

include_once ('misc_functions.php');
include_once ('config.php');



/**
*
* @brief Generate the URL for a PhpGedView individual.
*
* @param[in]   $n_id   PhpGedView ID for the person
*
* @returns   This function returns a string with the URL.
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
* @brief Generate the HTML link to a PhpGedView individual.
*
* @param[in]   $n_id   PhpGedView ID for the person
*
* @returns   This function returns a string with the HTML link.
*
*/

function pgv_indi_link($n_id)
{
	$url = pgv_indi_url($n_id);
	return html_link($n_id,$url);
}



/**
*
* @brief Generate the HTML link to a PhpGedView individual, using the person's name.
*
* @param[in,out]  $db     MySQLi database object
* @param[in]      $n_id   PhpGedView ID for the person
*
* @returns   This function returns a string with the HTML link.
*/

function pgv_indi_name_link($db, $n_id)
{
	if (($name = pgv_get_name($db,$n_id)) != null)
	{
		return html_link ($name, pgv_indi_url($n_id));
	}
	else
	{
		return $n_id;
	}
}



/**
*
* @brief Get the name of an individual.
*
* @param[in,out]  $db     MySQLi database object
* @param[in]      $n_id   PhpGedView ID for the person
*
* @returns    This function returns a string with the person's name.
*
*/

function pgv_get_name($db, $n_id)
{
	global $pgv_prefix;
	$query = "SELECT n_list FROM {$pgv_prefix}pgv_name WHERE n_id='$n_id'";
	if (!($result=$db->query($query))) die($db->error);
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



/**
*
* @brief Parse a GEDCOM record.
*
* @param[in]      $gedcom     GEDCOM record for one individual
* @param[in]      $level_1    Level 1 GEDCOM tag
* @param[in]      $level_2    Level 2 GEDCOM tag (optional)
*
* @returns    This function returns the parsed GEDCOM result, if successful.
*             Otherwise it returns null.
*
*/

function parse_gedcom ($gedcom, $level_1, $level_2 = null)
{
	if ($level_2 == null)
	{
		$pattern_1 = '/^1 ' . $level_1 . '\s(.*?)$/m';
		if (preg_match ($pattern_1, $gedcom, $match_1))
			$return_val = $match_1[1];
		else
			$return_val = null;
	}
	else
	{
		$pattern_1 = '/\n1 ' . $level_1 . '\s(.*?)(?:\n1|$)/s';
		$pattern_2 = '/^2 ' . $level_2 . '\s(.*?)$/m';
		if (preg_match ($pattern_1, $gedcom, $match_1))
			if (preg_match ($pattern_2, $match_1[1], $match_2))
				$return_val = $match_2[1];
			else
				$return_val = null;
		else
			$return_val = null;
	}
	
	return $return_val;
}



/**
*
* @brief Parse a GEDCOM XREF.
*
* @param[in]      $gedcom     GEDCOM record for one individual
* @param[in]      $level_1    Level 1 GEDCOM tag
*
* @returns    This function returns the GEDCOM XREF, if successful. Otherwise it returns null.
*
*/

function parse_gedcom_xref ($gedcom, $level_1)
{
	$pattern_1 = '/^1 ' . $level_1 . '\s@(.*?)@/m';
	if (preg_match ($pattern_1, $gedcom, $match_1))
		$return_val = $match_1[1];
	else
		$return_val = null;
	
	return $return_val;
}



/**
*
* @brief Handle the information about a PhpGedView individual.
*
*/

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
	
	/**
	*
	* @brief Constructor.
	*
	* @param[in,out]  $db     MySQLi database object
	* @param[in]      $n_id   PhpGedView ID for the person
	*
	*/
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
		
		//Load the GEDCOM record.
		
		$query = "SELECT i_gedcom FROM {$pgv_prefix}pgv_individuals WHERE i_id='$n_id'";
		if (!($result=$db->query($query))) die($db->error);
		if ($data = $result->fetch_object())
		{
			$i_gedcom = $data->i_gedcom;
		}
		else
		{
			$i_gedcom = null;
		}
		
		// Load birth and death information.
		
		$this->birth_date  = parse_gedcom ($i_gedcom, 'BIRT', 'DATE');
		$this->birth_place = parse_gedcom ($i_gedcom, 'BIRT', 'PLAC');
		$this->death_date  = parse_gedcom ($i_gedcom, 'DEAT', 'DATE');
		$this->death_place = parse_gedcom ($i_gedcom, 'DEAT', 'PLAC');

		// Load information about the parents.
		
		$f_id = parse_gedcom_xref ($i_gedcom, 'FAMC');
		
		if ($f_id != null)
		{
			$query = "SELECT f_husb, f_wife FROM {$pgv_prefix}pgv_families WHERE f_id='$f_id'";
			if (!($result=$db->query($query))) die($db->error);
			if ($row = $result->fetch_object())
			{
				// Father
				
				$this->father_id = $row->f_husb;
				if (($this->father_name = pgv_get_name($db, $this->father_id)) != null)
				{
					$this->father_link = html_link ($this->father_name, pgv_indi_url($this->father_id));
				}
				else
				{
					$this->father_link = $this->father_id;
				}
				
				// Mother
				
				$this->mother_id = $row->f_wife;
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
}
?> 
