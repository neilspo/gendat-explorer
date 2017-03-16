<?php
/**
*
* Search the NS birth, death and marriage records
*
* 3 April 2016
*
* This page produces an HTML form that allows users to ...
*
*/
/* 
include_once ('misc_functions.php');
include_once ('pgv_connect.php');




 */
include_once ('w_search.php');
include_once ('config.php');


$search = new w_search();

if(isset($_POST['submit']))
{
	$do_search = true;
	$LastName  = $_POST['LastName'];
	$FirstName = $_POST['FirstName'];
	
	$search->add_field ('LastName',  $LastName);
	$search->add_field ('FirstName', $FirstName);
}
else
{
	$do_search = false;
	$LastName  = null;
}





// Produce the form

echo <<<EOT
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<title>GenDat Search</title>
	<link rel="stylesheet" href="gendat.css">
</head>
<body>
	<form method="post" action="search.php" name="search">
		<div class="field">
			<label for="LastName">Surname</label>
			<input type="text" name="LastName" id="LastName" value="$LastName" >
		</div>
		<div class="field">
			<label for="FirstName">Given Name(s)</label>
			<input type="text" name="FirstName" id="FirstName" value="$FirstName" >
		</div>
		<div class="button">
			<input type="submit" name="submit" value="Submit" >
		</div>
	</form>
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

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}

// Execute the search.

$base_query = 'SELECT BirthID, LastName, FirstName FROM ns_births';
$max_rows   = 100;

$search->execute_search($db, $base_query, $max_rows);

while($row = $search->fetch_row())
	echo $row[LastName] . ' ' . $row[FirstName] . ' ' . $row[BirthID] . '<br>';

?>
