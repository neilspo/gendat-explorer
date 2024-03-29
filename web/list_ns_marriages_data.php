<?php

/**
*
* Produce a summary listing of the database table "ns_marriages_data".
*
* 27 January 2022
*
*/

include_once ('misc_functions.php');
include_once ('pgv_connect.php');
include_once ('config.php');

// Generate the page header.

html_header ('NS Marriage Records');

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
	}

// Issue an SQL query.

// WHERE (a.GroomLastName like 'Him%' OR a.BrideLastName like 'Him%') AND a.County = 'lunenburg'

// ORDER BY a.GroomLastName, a.Year, a.BrideLastName, a.MarriageID

$query = <<<EOT
SELECT a.*, b.groom, b.bride, b.n_id_g, b.n_id_b, c.f_id
FROM ns_marriages a
JOIN ns_marriages_data b USING (MarriageID)
LEFT JOIN phpgedview.pgv_families c ON  b.n_id_g = c.f_husb AND b.n_id_b = c.f_wife
ORDER BY a.GroomLastName, a.Year, a.BrideLastName, a.MarriageID
EOT;

if (!($result=$db->query($query))) die($db->error);

// Create a table from the rows of data.

table_start();

// Create the column headers.


$cell[0] = 'Marriage ID';
$cell[1] = 'URL';
$cell[2] = 'NSHVS Groom';
$cell[3] = 'NSHVS Bride';
$cell[4] = 'NSHVS Year';
$cell[5] = 'Groom';
$cell[6] = null;
$cell[7] = 'Bride';
$cell[8] = null;
$cell[9] = null;
table_row_header($cell);

// Output the data records.

while ($row = $result->fetch_object())
{
	$url = 'https://archives.novascotia.ca/vital-statistics/marriage/?ID=' . $row->MarriageID;

	$cell[0] = html_link($row->MarriageID, "edit_ns_marriages_data.php?id=$row->MarriageID");
	$cell[1] = html_link('Source', $url);
	$cell[2] = $row->GroomLastName . ', ' . $row->GroomFirstName;
	$cell[3] = $row->BrideLastName . ', ' . $row->BrideFirstName;
	$cell[4] = $row->Year;
	$cell[5] = $row->groom;
	$cell[6] = pgv_indi_link($row->n_id_g);
	$cell[7] = $row->bride;
	$cell[8] = pgv_indi_link($row->n_id_b);
	$cell[9] = $row->f_id;
	table_row($cell);
}
table_end();

// Generate the page trailer.

html_trailer();
?> 
