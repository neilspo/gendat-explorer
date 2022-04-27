<?php
/**
*
* Search Nova Scotia place names
*
* 19 June 2019
*
* This PHP script produces a HTML form that allows users to search for place names
* in the official Nova Scotia geoNAMES database.
*
*/

include_once ('config.php');
include_once ('w_search.php');

$county[ 0] = 'Any County';
$county[ 1] = 'Annapolis';
$county[ 2] = 'Antigonish';
$county[ 3] = 'Cape Breton';
$county[ 4] = 'Colchester';
$county[ 5] = 'Cumberland';
$county[ 6] = 'Digby';
$county[ 7] = 'Guysborough';
$county[ 8] = 'Halifax';
$county[ 9] = 'Hants';
$county[10] = 'Inverness';
$county[11] = 'Kings';
$county[12] = 'Lunenburg';
$county[13] = 'Pictou';
$county[14] = 'Queens';
$county[15] = 'Richmond';
$county[16] = 'Shelburne';
$county[17] = 'Victoria';
$county[18] = 'Yarmouth';

$selected = array_fill (0, 19, null);

// If the Submit button was pressed, then read the user input from the HTML form.

if(isset($_POST['submit']))
{
	$do_search   = true;
	$PlaceName   = $_POST['PlaceName'];
	$CountyIndex = $_POST['County'];
	$DataSet     = $_POST['DataSet'];
	$selected[$CountyIndex] = 'selected';
	if ($DataSet == 'curr')
	{
		$curr_selected = 'checked';
		$hist_selected = null;
	}
	else
	{
		$curr_selected = null;
		$hist_selected = 'checked';
	}
}
else
{
	$do_search     = false;
	$PlaceName     = null;
	$selected[0]   = 'selected';
	$curr_selected = 'checked';
	$hist_selected = null;
}

// Produce the form

echo <<<EOT
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>NS Place Names</title>
	<link rel="stylesheet" href="gendat.css">
</head>
<body>
	<h1>Nova Scotia Place Names</h1>
	<p>
		This page searches the 
		<a href="https://geonova.novascotia.ca/place-names">Nova Scotia GeoNAMES dataset</a>,
		which contains all current approved and offical geographic names for the Province of
		Nova Scotia. The named features in the dataset include Populated Places,
		Administrative Areas, Water and Terrain Features, Vegetation Areas,
		some Manmade Features, and Undersea Features.
	</p>
	<form method="post" action="search_ns_place_names.php" name="search">
		<fieldset>
		<legend>Search</legend>
		<div class="field">
			<label for="PlaceName">Place Name</label>
			<input type="text" name="PlaceName" id="PlaceName" value="$PlaceName" required >
		</div>
		<div class="field">
			<label for="County">County</label>
			<select name="County" id="County" >
				<option $selected[0] value="0"> $county[0]</option>
				<option $selected[1] value="1"> $county[1]</option>
				<option $selected[2] value="2"> $county[2]</option>
				<option $selected[3] value="3"> $county[3]</option>
				<option $selected[4] value="4"> $county[4]</option>
				<option $selected[5] value="5"> $county[5]</option>
				<option $selected[6] value="6"> $county[6]</option>
				<option $selected[7] value="7"> $county[7]</option>
				<option $selected[8] value="8"> $county[8]</option>
				<option $selected[9] value="9"> $county[9]</option>
				<option $selected[10] value="10"> $county[10]</option>
				<option $selected[11] value="11"> $county[11]</option>
				<option $selected[12] value="12"> $county[12]</option>
				<option $selected[13] value="13"> $county[13]</option>
				<option $selected[14] value="14"> $county[14]</option>
				<option $selected[15] value="15"> $county[15]</option>
				<option $selected[16] value="16"> $county[16]</option>
				<option $selected[17] value="17"> $county[17]</option>
				<option $selected[18] value="18"> $county[18]</option>
			</select>
		</div>
		<div class="field">
			<label>Data Set</label>
			<div class="radio-button-set">
				<input type="radio" name="DataSet" id="curr" value="curr" $curr_selected>
				<label for="curr">Current Approved Names</label><br>
				<input type="radio" name="DataSet" id="hist" value="hist" $hist_selected>
				<label for="hist">Historical Names</label>
			</div>
		</div>
		<div class="button">
			<input type="submit" name="submit" value="Submit" >
			<input type="submit" formnovalidate name="reset"  value="Reset"  >
		</div>	
		</fieldset>
	</form>
	<p>
		The search field can contain wildcards. For example, a search for u*musq* would return
		information about Upper Musquodoboit.
	</p>
	<hr style="width: 100%; height: 2px;">


EOT;

// If not yet ready to do the database search , then close out the HTML page and quit.

if (!$do_search)
{
	echo '</body>' . PHP_EOL . '</html>' . PHP_EOL;
	exit();
}

//-------------------------------------------------------------------------------------------------
// Now search the database.
//-------------------------------------------------------------------------------------------------

// Set a limit on the number of rows that the database query will return. This prevents an
// excessively broad search from returning thousands of matches.

$max_rows = 20;

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0)
	die('Unable to connect to database [' . $db->connect_error . ']');

// Make sure communications to and from the database server use the UTF-8 character set.

if (!($db->set_charset("utf8"))) die($db->error);

// Decide what to do, depending on the data-set selected.

$search = new w_search();
if ($DataSet == 'curr')
{
	$search->add_field ('GEONAME', $PlaceName);
	
	if ($CountyIndex > 0)
	{
		$CountyField = '%' . $county[$CountyIndex] . '%';
		$search->add_field ('COUNTY', $CountyField);
	}
	
	$base_query = 'SELECT SQL_CALC_FOUND_ROWS OBJECTID, GEONAME, LOCN_NARR, COUNTY, GENERIC_TM FROM ns_geonames';
}
else
{
	$search->add_field ('hist_name', $PlaceName);
	
	if ($CountyIndex > 0)
	{
		$CountyField = '%' . $county[$CountyIndex] . '%';
		$search->add_field ('county', $CountyField);
	}
	
	// Query fields: hist_name, county, hist_cgndb_id, ns_hist_name_id, curr_name
	//      sub-select 1: find all of the unused GeoNAMES place names with valid current names
	//      sub-select 2: find all of the other unused GeoNAMES place names
	//      sub-select 3: find historic names that are not included in GeoNAMES
	
	$base_query  = 'SELECT SQL_CALC_FOUND_ROWS * FROM (';
	$base_query .= '  SELECT a.hist_name, b.COUNTY AS county, a.hist_cgndb_id, NULL AS ns_hist_name_id,';
	$base_query .= '         b.GEONAME AS curr_name';
	$base_query .= '    FROM ns_geonames_hist a';
	$base_query .= '    JOIN ns_geonames b ON a.curr_cgndb_id = b.CGNDB_KEY';
	$base_query .= '  UNION';
	$base_query .= '  SELECT hist_name, location, hist_cgndb_id, NULL, NULL';
	$base_query .= '    FROM ns_geonames_hist';
	$base_query .= '    WHERE hist_cgndb_id NOT IN (';
	$base_query .= '      SELECT a.hist_cgndb_id';
	$base_query .= '      FROM ns_geonames_hist a';
	$base_query .= '      JOIN ns_geonames b ON a.curr_cgndb_id = b.CGNDB_KEY)';
	$base_query .= '  UNION';
	$base_query .= '  SELECT hist_name, hist_county, NULL, id, NULL';
	$base_query .= '    FROM ns_hist_name';
	$base_query .= '    WHERE curr_name IS NULL OR hist_name != curr_name) AS tbl';
	}

// Execute the database query.

$search->execute_search($db, $base_query, $max_rows);

// If the database query returned nothing, then produce a warning message.

$num_rows = $search->num_rows();
if ($num_rows == 0)
	echo '<p>No matches found</p>' . PHP_EOL;

// If the database query returned the maximum allowed number of rows, then find the total (unlimited)
// number of matches. If the total number of matches exceeded the limit, then report that.

if ($num_rows == $max_rows)
{
	$num_matches = $search->num_matches();
	if ($num_matches > $max_rows)
	{
		echo '<p>Your search matched ' . $num_matches . ' records, but only ' . $max_rows . ' are shown here.';
		echo ' Please refine the search</p>' .PHP_EOL;
	}
}

// Print out the results.

if ($DataSet == 'curr')
	{
	while ($row = $search->fetch_row())
	{
		$url = 'ns_place_name.php?id=' . $row['OBJECTID'];
		$link = '<a href="' . htmlspecialchars($url) . '">' . $row['GEONAME'] . '</a>';
		echo $link . PHP_EOL;
		echo "<ul>" . PHP_EOL;
		echo "	<li>$row[GENERIC_TM] &mdash; $row[LOCN_NARR]</li>" . PHP_EOL;
		echo "	<li>County(s) &mdash; $row[COUNTY] </li>" . PHP_EOL;
		echo "</ul>" . PHP_EOL;
	}
}
else
{
	while ($row = $search->fetch_row())
	{
		if (!empty($row['hist_cgndb_id']))
		{
			$url = 'ns_place_name_resc.php?id=' . $row['hist_cgndb_id'];
			$link = '<a href="' . htmlspecialchars($url) . '">' . $row['hist_name'] . '</a>';
			
			$message = 'Rescinded name';
			echo $link . PHP_EOL;
		}
		else
		{
			$url = 'ns_place_name_hist.php?id=' . $row['ns_hist_name_id'];
			$link = '<a href="' . htmlspecialchars($url) . '">' . $row['hist_name'] . '</a>';
			$message = 'Historical name';
			echo $link . PHP_EOL;
		}
		
		if (!empty($row['curr_name']))
			$message = $message . ', currently called &mdash; ' . $row['curr_name'];
		
		
		echo "<ul>" . PHP_EOL;
		echo "<li>$message</li>" . PHP_EOL;
		echo "	<li>County(s) &mdash; " . $row['county'] . "</li>" . PHP_EOL;
		echo "</ul>" . PHP_EOL;
	}
}

// Close out the HTML page.

echo PHP_EOL . '</body>' . PHP_EOL . '</html>' . PHP_EOL;

?>
