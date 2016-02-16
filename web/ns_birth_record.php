<?php
/**
*
* Edit one NS birth record
*
* 16 February 2016
*
* This page produces an HTML form that allows users to view and edit
* data transcribed from one Nova Scotia birth record.
*
*/

include_once ('misc_functions.php');
include_once ('pgv_connect.php');
include_once ('config.php');

// Generate the HTML page header.

html_header ('NS Birth Record', 'ns_birth_record.css');

// Get BirthID, the primary key for database access, which must be a positive integer.
//
// On the first call to this page, the value of BirthID is obtained from
// a URL parameter. If the page is then reentered, as a result of the
// "submit" button having been pressed, then the value of BirthID is obtained
// from a hidden input field, which must have been set on the previous call.
// 

if(isset($_POST['submit']))
{
	if(!isset($_POST['BirthID'])) die('Logic Error');
	$BirthID = $_POST['BirthID'];
}
else
{
	$BirthID = filter_input(INPUT_GET, 'id', FILTER_VALIDATE_INT);
	if($BirthID <= 0) die ('Invalid ID');
}

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}

// Get the NSHVS infromation about this record.

$query = "SELECT * FROM ns_births WHERE BirthID = $BirthID";
$result = $db->query($query);
if (!$result) die($db->error);
if ($row = $result->fetch_object())
{
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=birth&ID=' . $row->BirthID;
	echo html_link('NSHVS Record', $url) . '<br><br>' . PHP_EOL;
	echo $row->FirstName . ' ' . $row->LastName . ', born ' . $row->Year . ' in ' . $row->Place . ', ' . $row->County . ' County<br><br>';
}
else
{
	die('BirthID ' . $id . ' is not in the database.<br>');
}

// Define the data field names.

$field[0] = 'name';
$field[1] = 'birth_date';
$field[2] = 'birth_place';
$field[3] = 'father';
$field[4] = 'mother';
$field[5] = 'father_residence';
$field[6] = 'marriage_date';
$field[7] = 'marriage_place';
$field[8] = 'notes';
$field[9] = 'n_id';
$field[10] = 'n_id_f';
$field[11] = 'n_id_m';

// Get or update the data.


if(isset($_POST['submit']))
{
	//***************************************************************
	// Process new data obtained after the HTML form was submitted.
	//***************************************************************
	
	// Get the new field values from the HTML form input fields.
	
	for ($i=0; $i<count($field); $i++) ${$field[$i]} = $_POST[$field[$i]];
	
	// Change blank fields to null.
	
	for ($i=0; $i<count($field); $i++) if (${$field[$i]} == "") ${$field[$i]} = null;
		
	// If this record is not already in the database, then create it.
		
	if ($_POST['new_record'] == 'true')
	{
		$query = "INSERT INTO ns_births_data (BirthID) VALUES ($BirthID)";
		if (!$db->query($query)) die($db->error);
	}
	$new_record = 'false';

	// Store the new data from the HTML form in the database. 

	// *** Set up a prepared SQL statement.

	$query = "UPDATE ns_births_data SET $field[0]=?";
	for ($i=1; $i<count($field); $i++) $query = $query . ", $field[$i]=?";
	$query = $query . " WHERE BirthID=$BirthID";
	if (!($stmt=$db->prepare($query))) die($db->error);
	
	// *** Bind the variables.
	
	$types       = '';
	$arg_list[0] = &$types;
	for ($i=0; $i<count($field); $i++)
	{
		$types          = $types . 's';
		$arg_list[$i+1] = &${$field[$i]};
	}
	if (!call_user_func_array(array($stmt, "bind_param"),$arg_list))
		die ('Bind failed');
	
	// *** Execute the prepare statement.
	
	if (!$stmt->execute())
		die ("Execute failed: (" . $stmt->errno . ") " . $stmt->error);
	
	// *** Release the prepared statement.
	
	$stmt->close();
}
  else
{
	$query = "SELECT * FROM ns_births_data WHERE BirthID = $BirthID";
	if (!($result=$db->query($query))) die($db->error);
	if ($row = $result->fetch_object())
	{
		// This record already exists in the database.
		
		$new_record = 'false';
		
		// Get the field values from the database query result set.
		
		for ($i=0; $i<count($field); $i++) ${$field[$i]} = $row->{$field[$i]};
	}
	else
	{
		// This record is not in the database and will need to be added when the from is submitted.
		
		$new_record = 'true';
		
		// The field values are all null.
		
		for ($i=0; $i<count($field); $i++) ${$field[$i]} = null;
	}
}

// Get the PGV data on this person, the father and the mother.

$pgv_ind   = new pgv_ind($db, $n_id);
$pgv_ind_f = new pgv_ind($db, $n_id_f);
$pgv_ind_m = new pgv_ind($db, $n_id_m);


// Generate the form.

echo '<form action="ns_birth_record.php" method="post">' . PHP_EOL;

table_start();

$cell[0] = NULL;
$cell[1] = 'NSHVS';
$cell[2] = 'Database';
table_row_header($cell);

$cell[0] = '<label for="name">Name</label>';
$cell[1] = '<input type="text" name="name" id="name" value="' . $name . '" >';
$cell[2] = $pgv_ind->link;
table_row($cell);

$cell[0] = '<label for="birth_date">Date of Birth</label>';
$cell[1] = '<input type="text" name="birth_date" id="birth_date" value="' . $birth_date . '" >';
$cell[2] = $pgv_ind->birth_date;
table_row($cell);

$cell[0] = '<label for="birth_date">Place of Birth</label>';
$cell[1] = '<input type="text" name="birth_place" id="birth_place" value="' . $birth_place . '" >';
$cell[2] = $pgv_ind->birth_place;
table_row($cell);

$cell[0] = '<label for="father">Father</label>';
$cell[1] = '<input type="text" name="father" id="father" value="' . $father . '" >';
$cell[2] = $pgv_ind_f->link;
table_row($cell);

$cell[0] = '<label for="mother">Mother</label>';
$cell[1] = '<input type="text" name="mother" id="mother" value="' . $mother . '" >';
$cell[2] = $pgv_ind_m->link;
table_row($cell);

$cell[0] = '<label for="father_residence">Father\'s Residence</label>';
$cell[1] = '<input type="text" name="father_residence" id="father_residence" value="' . $father_residence . '" >';
$cell[2] = null;
table_row($cell);

$cell[0] = '<label for="marriage_date">Parents\' Date of Marriage</label>';
$cell[1] = '<input type="text" name="marriage_date" id="marriage_date" value="' . $marriage_date . '" >';
$cell[2] = null;
table_row($cell);

$cell[0] = '<label for="marriage_place">Parents\' Place of Marriage</label>';
$cell[1] = '<input type="text" name="marriage_place" id="marriage_place" value="' . $marriage_place . '" >';
$cell[2] = null;
table_row($cell);

table_end();

echo <<<EOT
<label for="notes">Notes</label>
<input type="text" name="notes" id="notes" value="$notes" >
<span style="white-space: nowrap">
  <label for="n_id">Child n_id</label>
  <input type="text" name="n_id" id="n_id" value="$n_id"  style="width:100px;">
</span>
<span style="white-space: nowrap">
  <label for="n_id_f">Father n_id</label>
  <input type="text" name="n_id_f" id="n_id_f" value="$n_id_f"  style="width:100px;">
</span>
<span style="white-space: nowrap">
  <label for="n_id_m">Mother n_id</label>
  <input type="text" name="n_id_m" id="n_id_m" value="$n_id_m" style="width:100px;">
</span>
<br><br>
<input type="submit" name="submit" value="Submit" >
<input type="hidden" name="BirthID" value="$BirthID" >
<input type="hidden" name="new_record" value="$new_record">
</form>
</body></html>
EOT;

?>
