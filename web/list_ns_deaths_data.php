<?php

/**
*
* Produce a summary listing of the database table "ns_deaths_data".
*
*/

include_once ('misc_functions.php');
include_once ('config.php');

// Generate the page header.

html_header ('NS Death Records');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
	}

// Issue an SQL query.

$query = <<<EOT
SELECT a.*, b.name
FROM ns_deaths a
JOIN ns_deaths_data b USING (Deathid)
ORDER BY a.LastName, a.Year, a.FirstName, a.Deathid
EOT;

if (!($result=$db->query($query))) die($db->error);

// Create a table from the rows of data.

table_start();

// Create the column headers.


$cell[0] = 'Death ID';
$cell[1] = 'Last Name';
$cell[2] = 'First Name';
$cell[3] = 'Place';
$cell[4] = 'Year';
$cell[5] = 'Name';
$cell[6] = 'URL';
table_row_header($cell);

// Output the data records.

while ($row = $result->fetch_object())
{
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=death&ID=' . $row->Deathid;

	$cell[0] = html_link($row->Deathid, "edit_ns_deaths_data.php?id=$row->Deathid");
	$cell[1] = $row->LastName;
	$cell[2] = $row->FirstName;
	$cell[3] = $row->Place;
	$cell[4] = $row->Year;
	$cell[5] = $row->name;
	$cell[6] = html_link('Source', $url);
	table_row($cell);
}
table_end();

// Generate the page trailer.

html_trailer();
?> 
