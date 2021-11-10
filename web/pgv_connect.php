<?php

/**
* @file
*
* @brief Connect to a PhpGedView database.
*
* @date 10 November 2021
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
		
		// PGV uses the string '@N.N.' or '@P.N.' to represent an unknown name.  Replace that string
		// with '(unknown)'. Return NULL if the individual cannot be found in the database.
		
		$name = str_replace ('@N.N.', '(unknown)', $name);
		$name = str_replace ('@P.N.', '(unknown)', $name);
		return $name;
	}
	else
	{
		return null;
	}
}

/**
*
* @brief Find all of a person's children.
*
* @param[in,out]  $db     MySQLi database object
* @param[in]      $n_id   PhpGedView ID for the person
*
* @returns   This function returns an array containing the PhpGedView IDs for the children.
*
*/
function pgv_get_children ($db, $n_id)
{
$query = <<<EOQ
SELECT i_id
FROM phpgedview.pgv_individuals AS i
JOIN (
    SELECT CONCAT("%1 FAMC @", f_id, "@%") famc
    FROM phpgedview.pgv_families
    WHERE f_husb='$n_id' OR f_wife='$n_id') AS f
WHERE i.i_gedcom LIKE f.famc
EOQ;
	
	if (!($result=$db->query($query))) die($db->error);
	
	$children = array();
	while ($data = $result->fetch_object())
	{
		$children[] = $data->i_id;
	}
	
	return $children;
}



/**
*
* @brief Parse a GEDCOM record.
*
* @param[in]      $gedcom     GEDCOM record for one individual
* @param[in]      $level_1    Level 1 GEDCOM tag
* @param[in]      $level_2    Level 2 GEDCOM tag (optional)
* @param[in]      $level_3    Level 3 GEDCOM tag (optional)

*
* @returns    This function returns the data string, if the requested GEDCOM tags were found.
*             Otherwise it returns null.
*
*/

function parse_gedcom ($gedcom, $level_1, $level_2 = null, $level_3 = null)
{
	$pattern_1 = '/\n1 ' . $level_1 . '(.*?)(?:\n1|$)/s';
	if (preg_match ($pattern_1, $gedcom, $match_1))
	{
		if ($level_2 == null)
		{
			$return_val = trim($match_1[1]);
		}
		else
		{
			$pattern_2 = '/\n2 ' . $level_2 . '(.*?)(?:\n2|\n1|$)/s';
			if (preg_match ($pattern_2, $match_1[1], $match_2))
			{
				if ($level_3 == null)
				{
					$return_val = trim($match_2[1]);
				}
				else
				{
					$pattern_3 = '/\n3 ' . $level_3 . '(.*?)(?:\n3|\n2|\n1|$)/s';
					if (preg_match ($pattern_3, $match_2[1], $match_3))
					{
						$return_val = trim($match_3[1]);
					}
					else
					{
						$return_val = null;
					}
				}
			}
			else
			{
				$return_val = null;
			}
		}
	}
	else
	{
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
* @param[in]      $level_2    Level 2 GEDCOM tag (optional)
* @param[in]      $level_3    Level 3 GEDCOM tag (optional)
*
* @returns    This function returns the GEDCOM XREF, if the requested GEDCOM tags were found.
*             Otherwise it returns null.
*
*/

function parse_gedcom_xref ($gedcom, $level_1, $level_2 = null, $level_3 = null)
{
	$test_string = parse_gedcom ($gedcom, $level_1, $level_2, $level_3);
	if ($test_string != null)
	{
		// A valid GEDCOM XREF must be delimited by '@' characters. Find the actual XREF
		// and strip anything else off.
		
		$pattern = '/@(.*?)@/';
		if (preg_match ($pattern, $test_string, $match_1))
			$return_val = $match_1[1];
		else
			$return_val = null;
	}
	else
	{
		$return_val = null;
	}
	
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
			return;
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
				
				$this->father_id   = $row->f_husb;
				$this->father_link = pgv_indi_name_link($db, $this->father_id);
				
				// Mother
				
				$this->mother_id   = $row->f_wife;
				$this->mother_link = pgv_indi_name_link($db, $this->mother_id);
			}
		}
	}
}
?> 
