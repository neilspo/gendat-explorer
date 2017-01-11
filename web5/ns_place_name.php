<?php
/**
*
* Show information about a particular Nova Scotia place name
*
* 10 January 2017
*
* This PHP script produces a page that shows information from one record
* in the official Nova Scotia geoNAMES database.
*
* Example call:
*
*     ns_place_name.php?id=12345
*
*/

include_once ('config.php');


// Get $objectid, the primary key for database access, which must be a positive integer.

$objectid = filter_input(INPUT_GET, 'id', FILTER_VALIDATE_INT);
if($objectid <= 0) die ('Invalid ID');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0)
	die('Unable to connect to database [' . $db->connect_error . ']');

// Make sure communications to and from the database server use the UTF-8 character set.

if (!($db->set_charset("utf8"))) die($db->error);

// Get this record from the database.

$query = "SELECT * FROM ns_geonames WHERE OBJECTID = $objectid";
if (!($result=$db->query($query))) die($db->error);

if (!($row = $result->fetch_object()))
	die('OBJECTID ' . $id . ' is not in the database.<br>');

// Create a link to a map showing this location, using Open Street Map.

$url = 'http://www.openstreetmap.org/?mlat=' . $row->NAD83_LAT . '&mlon=' . $row->NAD83_LON . '&zoom=14';
$map = '<a href="' . htmlspecialchars($url) . '">Map</a>';

// Create a link to a definition of the geographic feature type.

$feature_type = '<a href="http://geogratis.gc.ca/services/geoname/en/codes/generic/' . 
				$row->GENERIC_CD . '">' . $row->GENERIC_TM .  '</a>';

// Create a link to more information on the Natural Resources Canada web site.

$url = 'http://www4.rncan.gc.ca/search-place-names/unique?id=' . $row->CGNDB_KEY;
$cgndb_link = '<a href="' . htmlspecialchars($url) . '">record</a>';

// Header tags for the HTML page.

echo <<<EOT
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>$row->GEONAME</title>
	<link rel="stylesheet" href="gendat.css">
</head>
<body>
	<h1>$row->GEONAME</h1>
	<div class="text-left">
		CGNDB Key
	</div>
	<div class="text-right">
		$row->CGNDB_KEY
	</div>
	
	<div class="text-left">
		Location
	</div>
	<div class="text-right">
		$map - $row->LOCN_NARR
	</div>
	
	<div class="text-left">
		Type
	</div>
	<div class="text-right">
		$feature_type
	</div>
	
	<div class="text-left">
		Description
	</div>
	<div class="text-right">
		$row->CONCISE_DS
	</div>
	
	<div class="text-left">
		Admistrative Location
	</div>
	<div class="text-right">
		$row->ADMIN_LOC
	</div>
	
	<div class="text-left">
		County(s)
	</div>
	<div class="text-right">
		$row->COUNTY
	</div>
	
	<div class="text-left">
		Status
	</div>
	<div class="text-right">
		$row->STATUS_DS
	</div>
	
	<div class="text-left">
		NTS Map Number(s)
	</div>
	<div class="text-right">
		$row->NTS_MAP
	</div>
	
	<div class="text-left">
		Jurisdictional Narrative
	</div>
	<div class="text-right">
		$row->JURR_NARR
	</div>
	
	<div class="text-left">
		Historical Narrative
	</div>
	<div class="text-right">
		$row->ORIG_NARR
	</div>
	<p>
	For more information, see this $cgndb_link in the Canadian Geographical Names Data Base (CGNDB).
EOT;

?>
