<?php

/**
*
* Produce a summary listing of the database table "ns_births_data".
*
*/

include_once ('misc_functions.php');
include_once ('pgv_connect.php');
include_once ('config.php');

// Generate the page header.

html_header ('NS Birth Records');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
	}

// Issue an SQL query.

$query = <<<EOT
SELECT a.*, b.name, b.n_id
FROM ns_births a
JOIN ns_births_data b USING (BirthID)
ORDER BY a.LastName, a.Year, a.FirstName, a.BirthID
EOT;

if (!($result=$db->query($query))) die($db->error);

// Create a table from the rows of data.

table_start();

// Create the column headers.


$cell[0] = 'Birth ID';
$cell[1] = 'URL';
$cell[2] = 'Last Name';
$cell[3] = 'First Name';
$cell[4] = 'Place';
$cell[5] = 'Year';
$cell[6] = 'Name';
$cell[7] = null;
table_row_header($cell);

// Output the data records.

while ($row = $result->fetch_object())
{
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=birth&ID=' . $row->BirthID;

	$cell[0] = html_link($row->BirthID, "edit_ns_births_data.php?id=$row->BirthID");
	$cell[1] = html_link('Source', $url);
	$cell[2] = $row->LastName;
	$cell[3] = $row->FirstName;
	$cell[4] = $row->Place;
	$cell[5] = $row->Year;
	$cell[6] = $row->name;
	$cell[7] = pgv_indi_link($row->n_id);
	table_row($cell);
}
table_end();

// Generate the page trailer.

html_trailer();
?> 
