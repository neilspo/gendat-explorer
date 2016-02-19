<?php

/**
*
* Produce a summary listing of the database table "ns_marriages_data".
*
*/

include_once ('misc_functions.php');
include_once ('config.php');

// Generate the page header.

html_header ('NS Marriage Records');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
	}

// Issue an SQL query.

$query = <<<EOT
SELECT a.*, b.groom, b.bride
FROM ns_marriages a
JOIN ns_marriages_data b USING (MarriageID)
ORDER BY a.GroomLastName, a.Year, a.BrideLastName, a.MarriageID
EOT;

$result = $db->query($query);

// Create a table from the rows of data.

table_start();

// Create the column headers.


$cell[0] = 'Marriage ID';
$cell[1] = 'URL';
$cell[2] = 'NSHVS Groom';
$cell[3] = 'NSHVS Bride';
$cell[4] = 'NSHVS Year';
$cell[5] = 'Groom';
$cell[6] = 'Bride';
table_row_header($cell);

// Output the data records.

while ($row = $result->fetch_object())
{
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=marriage&ID=' . $row->MarriageID;

	$cell[0] = html_link($row->MarriageID, "edit_ns_marriages_data.php?id=$row->MarriageID");
	$cell[1] = html_link('Source', $url);
	$cell[2] = $row->GroomLastName . ', ' . $row->GroomFirstName;
	$cell[3] = $row->BrideLastName . ', ' . $row->BrideFirstName;
	$cell[4] = $row->Year;
	$cell[5] = $row->groom;
	$cell[6] = $row->bride;
	table_row($cell);
}
table_end();

// Generate the page trailer.

html_trailer();
?> 
