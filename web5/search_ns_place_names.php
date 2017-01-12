<?php
/**
*
* Search Nova Scotia place names
*
* 12 January 2017
*
* This PHP script produces a HTML form that allows users to search for place names
* in the official Nova Scotia geoNAMES database.
*
*/

include_once ('config.php');

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
	$selected[$CountyIndex] = 'selected';
}
else
{
	$do_search   = false;
	$PlaceName   = null;
	$selected[0] = 'selected';
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

// Create the template SQL statement.

$query = 'SELECT SQL_CALC_FOUND_ROWS OBJECTID, GEONAME, LOCN_NARR, COUNTY FROM ns_geonames WHERE GEONAME';
if (preg_match ('/[*%]/', $PlaceName))
{
	$query .= ' LIKE ?';
	$PlaceName = str_replace ('*', '%', $PlaceName);
}
else
{
	$query .= '=?';
}
if ($CountyIndex > 0)
	$query .= " AND COUNTY LIKE '%" . $county[$CountyIndex] . "%'";
$query .= ' LIMIT ' . $max_rows;

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0)
	die('Unable to connect to database [' . $db->connect_error . ']');

// Make sure communications to and from the database server use the UTF-8 character set.

if (!($db->set_charset("utf8"))) die($db->error);

// Have the MySQL server prepare the statement.

if (!($stmt=$db->prepare($query))) die($db->error);
	
// Bind the search parameter.

if (!($stmt->bind_param('s', $PlaceName))) die($db->error);

// Execute the prepared statement.

if (!($stmt->execute())) die($db->error);

// Get the number of rows returned by the SQL query.
// Note: The call to store_result() is only needed here to get the correct number of rows.

$stmt->store_result();
$num_rows = $stmt->num_rows;

// If the database query returned nothing, then report that and quit.

if ($num_rows == 0)
{
	echo '<p>No matches found</p>' . PHP_EOL;
	echo '</body>' . PHP_EOL . '</html>' . PHP_EOL;
	exit();
}

// If the database query returned the maximum allowed number of rows, then find the total (unlimited)
// number of matches and produce a warning message if needed.

if ($num_rows == $max_rows)
{
	// Ask the MySQL server for total number of matches.
	
	if (!($result = $db->query("SELECT FOUND_ROWS()"))) die($db->error);
	$temp_array  = $result->fetch_row();
	$num_matches = $temp_array[0];
	
	// If the total number of matches exceeded the limit, then report that.
	
	if ($num_matches > $max_rows)
	{
		echo '<p>Your search matched ' . $num_matches . ' records, but only ' . $max_rows . ' are shown here.';
		echo ' Please refine the search</p>' .PHP_EOL;
	}
}

// Bind the result variables.

$stmt->bind_result($objectid, $geoname, $locn_narr, $county_name);

// Print out the results.

while ($stmt->fetch())
{
	$url = 'ns_place_name.php?id=' . $objectid;
	$link = '<a href="' . htmlspecialchars($url) . '">' . $geoname . '</a>';
	echo <<<EOT
	$link
	<ul>
		<li>$locn_narr</li>
		<li>County(s) &mdash; $county_name </li>
	</ul>
EOT;
}

// --- Release the prepared statement.

$stmt->close();

// Close out the HTML page.

echo PHP_EOL . '</body>' . PHP_EOL . '</html>' . PHP_EOL;

?>
