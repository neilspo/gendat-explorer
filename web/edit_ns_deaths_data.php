<?php
/**
*
* Edit one NS death record
*
* 9 March 2016
*
* This page produces an HTML form that allows users to view and edit
* data transcribed from one Nova Scotia death record.
*
*/

/**
*
* Prepare a HTML form <input> element.
*
* Note: This function will not work correctly if there is a variable in 
*       the main program called $my_field_name.
*
*/

function form_input ($my_field_name)
{
	global ${$my_field_name};
	return "<input type='text' name='$my_field_name' id='$my_field_name' value='" . ${$my_field_name} . "' >";
}


include_once ('misc_functions.php');
include_once ('pgv_connect.php');
include_once ('config.php');

// Generate the HTML page header.

html_header ('NS Death Record', 'gendat.css');

// Get Deathid, the primary key for database access, which must be a positive integer.
//
// On the first call to this page, the value of Deathid is obtained from
// a URL parameter. If the page is then reentered, as a result of the
// "submit" button having been pressed, then the value of Deathid is obtained
// from a hidden input field, which must have been set on the previous call.
// 

if(isset($_POST['submit']))
{
	if(!isset($_POST['Deathid'])) die('Logic Error');
	$Deathid = $_POST['Deathid'];
}
else
{
	$Deathid = filter_input(INPUT_GET, 'id', FILTER_VALIDATE_INT);
	if($Deathid <= 0) die ('Invalid ID');
}

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}

// Get the NSHVS infromation about this record.

$query = "SELECT * FROM ns_deaths WHERE Deathid = $Deathid";
if (!($result=$db->query($query))) die($db->error);

if ($row = $result->fetch_object())
{
	// Create the URL of this record at Nova Scotia Historical Vital Statistics
	
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=death&ID=' . $row->Deathid;
	
	// Some dates have month and day set, others don't.
	
	$date = $row->Year;
	if ($row->Month <> null)
	{
		$date = date('F', mktime(0, 0, 0, $row->Month)) . ' ' . $date;
		if ($row->Day <> null) $date = $row->Day . ' ' . $date;
	}
	
	// Display the results
	
	echo html_link('NSHVS Record', $url) . '<br><br>' . PHP_EOL;
	echo $row->FirstName . ' ' . $row->LastName . ', died ' . $date . ' in ';
	if ($row->Place <> null) echo $row->Place . ', ';
	echo $row->County . ' County<br><br>';
}
else
{
	die('Deathid ' . $id . ' is not in the database.<br>');
}

// Define the data field names.

$field[0]  = 'name';
$field[1]  = 'death_date';
$field[2]  = 'death_place';
$field[3]  = 'death_age';
$field[4]  = 'death_residence';
$field[5]  = 'birth_date';
$field[6]  = 'birth_place';
$field[7]  = 'mar_status';
$field[8]  = 'spouse';
$field[9]  = 'father';
$field[10] = 'father_birthplace';
$field[11] = 'mother';
$field[12] = 'mother_birthplace';
$field[13] = 'informant';
$field[14] = 'notes';
$field[15] = 'n_id';

// Get or update the data.

if(isset($_POST['submit']))
{
	//*************************************************************************
	// The submit button on the HTML form has been pressed.
	//
	// Read the HTML form input fields and store these data in the database.
	//*************************************************************************
	
	for ($i=0; $i<count($field); $i++)
	{
		// Make sure the needed HTML input form fields were set. If not, then this is a bug.
		
		if (!isset($_POST[$field[$i]])) die("Bug: field '$field[$i]' was not set.");
		
		// Get the new field values from the HTML form input fields (with leading
		// and trailing white space trimmed).
		
		${$field[$i]} = trim($_POST[$field[$i]]);
		
		// Change blank fields to null.
		
		if (${$field[$i]} == "") ${$field[$i]} = null;
	}
	
	// If this record is not already in the database, then create it.
		
	if ($_POST['new_record'] == 'true')
	{
		$query = "INSERT INTO ns_deaths_data (Deathid) VALUES ($Deathid)";
		if (!$db->query($query)) die($db->error);
	}
	$new_record = 'false';

	// Store the new data from the HTML form in the database. The following
	// code does this using prepared SQL statements. This ensures that all
	// data in the database has been properly escaped and eliminates any
	// possibility of SQL malicious code injection.

	// --- Create the template SQL query for the prepared statement.

	$query = "UPDATE ns_deaths_data SET $field[0]=?";
	for ($i=1; $i<count($field); $i++) $query = $query . ", $field[$i]=?";
	$query = $query . " WHERE Deathid=$Deathid";
	
	// --- Have the database prepare the statement.
	
	if (!($stmt=$db->prepare($query))) die($db->error);
	
	// --- Bind the variables.
	
	$types       = '';
	$arg_list[0] = &$types;
	for ($i=0; $i<count($field); $i++)
	{
		$types          = $types . 's';
		$arg_list[$i+1] = &${$field[$i]};
	}
	if (!call_user_func_array(array($stmt, "bind_param"),$arg_list))
		die ('Bind failed');
	
	// --- Execute the prepare statement.
	
	if (!$stmt->execute()) die ("Execute failed");
	
	// --- Release the prepared statement.
	
	$stmt->close();
}
  else
{
	//*************************************************************************
	// Prepare to show the HTML form for the first time.
	//*************************************************************************
	
	// Get the data from the database.
	
	$query = "SELECT * FROM ns_deaths_data WHERE Deathid = $Deathid";
	if (!($result=$db->query($query))) die($db->error);
	
	// Prepare the data values that will be shown in the HTML form.
	
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
		
		// Set the field values to null.
		
		for ($i=0; $i<count($field); $i++) ${$field[$i]} = null;
	}
}

// Get the PGV data on this person.

$pgv_ind   = new pgv_ind($db, $n_id);

// Before generating the form, convert any characters in the strings that have special
// significance in HTML to the correct HTML entities

for ($i=0; $i<count($field); $i++) ${$field[$i]} = htmlspecialchars(${$field[$i]});

// Generate the form.

echo '<form action="edit_ns_deaths_data.php" method="post">' . PHP_EOL;

table_start();

$cell[0] = NULL;
$cell[1] = 'NSHVS';
$cell[2] = 'Database';
table_row_header($cell);

$cell[0] = 'Name';
$cell[1] = form_input('name');
$cell[2] = $pgv_ind->link;
table_row($cell);

$cell[0] = 'Date of Death';
$cell[1] = form_input('death_date');
$cell[2] = $pgv_ind->death_date;
table_row($cell);

$cell[0] = 'Place of Death';
$cell[1] = form_input('death_place');
$cell[2] = $pgv_ind->death_place;
table_row($cell);

$cell[0] = 'Age';
$cell[1] = form_input('death_age');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Residence';
$cell[1] = form_input('death_residence');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Date of Birth';
$cell[1] = form_input('birth_date');
$cell[2] = $pgv_ind->birth_date;
table_row($cell);

$cell[0] = 'Place of Birth';
$cell[1] = form_input('birth_place');
$cell[2] = $pgv_ind->birth_place;
table_row($cell);

$cell[0] = 'Marital Status';
$cell[1] = form_input('mar_status');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Spouse';
$cell[1] = form_input('spouse');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Father';
$cell[1] = form_input('father');
$cell[2] = $pgv_ind->father_link;
table_row($cell);

$cell[0] = 'Father\'s Place of Birth';
$cell[1] = form_input('father_birthplace');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Mother';
$cell[1] = form_input('mother');
$cell[2] = $pgv_ind->mother_link;
table_row($cell);

$cell[0] = 'Mother\'s Place of Birth';
$cell[1] = form_input('mother_birthplace');
$cell[2] = null;
table_row($cell);

$cell[0] = 'Informant';
$cell[1] = form_input('informant');
$cell[2] = null;
table_row($cell);

table_end();

echo <<<EOT
<label for="notes">Notes</label>
<input type="text" name="notes" id="notes" value="$notes" >
<span style="white-space: nowrap">
  <label for="n_id">n_id</label>
  <input type="text" name="n_id" id="n_id" value="$n_id"  style="width:100px;">
</span>
<br><br>
<input type="submit" name="submit" value="Submit" >
<input type="hidden" name="Deathid" value="$Deathid" >
<input type="hidden" name="new_record" value="$new_record">
</form>
</body></html>
EOT;

?>
