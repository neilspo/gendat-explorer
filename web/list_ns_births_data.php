<?php

include_once ('misc_functions.php');
include_once ('config.php');

// Generate the page header.

html_header ('NS Birth Records');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
	}

// Issue an SQL query.

$query = "SELECT a.*, b.name FROM ns_births a JOIN ns_births_data b USING (BirthID) ORDER BY a.LastName, a.Year, a.FirstName, a.BirthID";
//$query = "SELECT a.*, b.name FROM ns_births a JOIN ns_births_data b USING (BirthID)";
$result = $db->query($query);

// Create a table from the rows of data.

table_start();

// Create the column headers.


$cell[0] = 'Last Name';
$cell[1] = 'First Name';
$cell[2] = 'Place';
$cell[3] = 'Year';
$cell[4] = 'Name';
$cell[5] = 'URL';
$cell[6] = null;
table_row_header($cell);

// Output the data records.

while ($row = $result->fetch_object())
{
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=birth&ID=' . $row->BirthID;

	$cell[0] = $row->LastName;
	$cell[1] = $row->FirstName;
	$cell[2] = $row->Place;
	$cell[3] = $row->Year;
	$cell[4] = $row->name;
	$cell[5] = html_link('Source', $url);
	$cell[6] = html_link($row->BirthID, "edit_ns_births_data.php?id=$row->BirthID");
	table_row($cell);
}
table_end();

// Generate the page trailer.

html_trailer();
?> 
