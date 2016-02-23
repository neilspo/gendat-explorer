<?php
/**
*
* Edit one NS marriage record
*
* 23 February 2016
*
* This page produces an HTML form that allows users to view and edit
* data transcribed from one Nova Scotia marriage record.
*
*/

/**
*
* Prepare a HTML form <input> element.
*
*/

function form_input ($name)
{
	global ${$name};
	return "<input type='text' name='$name' id='$name' value='" . ${$name} . "' >";
}


include_once ('misc_functions.php');
include_once ('pgv_connect.php');
include_once ('config.php');

// Generate the HTML page header.

html_header ('NS Marriage Record', 'gendat.css');

// Get MarriageID, the primary key for database access, which must be a positive integer.
//
// On the first call to this page, the value of MarriageID is obtained from
// a URL parameter. If the page is then reentered, as a result of the
// "submit" button having been pressed, then the value of MarriageID is obtained
// from a hidden input field, which must have been set on the previous call.
// 

if(isset($_POST['submit']))
{
	if(!isset($_POST['MarriageID'])) die('Logic Error');
	$MarriageID = $_POST['MarriageID'];
}
else
{
	$MarriageID = filter_input(INPUT_GET, 'id', FILTER_VALIDATE_INT);
	if($MarriageID <= 0) die ('Invalid ID');
}

// Connect to the database

$db = new mysqli($db_host, $db_user, $db_pass, $db_name);
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
}

// Get the NSHVS infromation about this record.

$query = "SELECT * FROM ns_marriages WHERE MarriageID = $MarriageID";
if (!($result=$db->query($query))) die($db->error);

if ($row = $result->fetch_object())
{
	// Create the URL of this record at Nova Scotia Historical Vital Statistics
	
	$url = 'https://www.novascotiagenealogy.com/ItemView.aspx?ImageFile=' . $row->RegBook . '-' . $row->RegPage . '&Event=marriage&ID=' . $row->MarriageID;
	
	// Some dates have month and day set, others don't.
	
	$date = $row->Year;
	if ($row->Month <> null)
	{
		$date = date('F', mktime(0, 0, 0, $row->Month)) . ' ' . $date;
		if ($row->Day <> null) $date = $row->Day . ' ' . $date;
	}
	
	// Display the results
	
	echo html_link('NSHVS Record', $url) . '<br><br>' . PHP_EOL;
	echo $row->BrideFirstName . ' ' . $row->BrideLastName . ' and ';
	echo $row->GroomFirstName . ' ' . $row->GroomLastName . ', married ' . $date . ' in ';
	if ($row->Place <> null) echo $row->Place . ', ';
	echo $row->County . ' County<br><br>';
}
else
{
	die('MarriageID ' . $id . ' is not in the database.<br>');
}

// Define the data field names.

$field[0]  = 'groom';
$field[1]  = 'groom_age';
$field[2]  = 'groom_status';
$field[3]  = 'groom_residence';
$field[4]  = 'groom_birthplace';
$field[5]  = 'groom_occupation';
$field[6]  = 'groom_father';
$field[7]  = 'groom_father_birthplace';
$field[8]  = 'groom_mother';
$field[9]  = 'groom_mother_birthplace';
$field[10] = 'bride';
$field[11] = 'bride_age';
$field[12] = 'bride_status';
$field[13] = 'bride_residence';
$field[14] = 'bride_birthplace';
$field[15] = 'bride_occupation';
$field[16] = 'bride_father';
$field[17] = 'bride_father_birthplace';
$field[18] = 'bride_mother';
$field[19] = 'bride_mother_birthplace';
$field[20] = 'date';
$field[21] = 'place';
$field[22] = 'notes';
$field[23] = 'n_id_g';
$field[24] = 'n_id_b';


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
		
		// Get the new field values from the HTML form input fields.
		
		${$field[$i]} = $_POST[$field[$i]];
		
		// Change blank fields to null.
		
		if (${$field[$i]} == "") ${$field[$i]} = null;
	}
	
	// If this record is not already in the database, then create it.
		
	if ($_POST['new_record'] == 'true')
	{
		$query = "INSERT INTO ns_marriages_data (MarriageID) VALUES ($MarriageID)";
		if (!$db->query($query)) die($db->error);
	}
	$new_record = 'false';

	// Store the new data from the HTML form in the database. The following
	// code does this using prepared SQL statements. This ensures that all
	// data in the database has been properly escaped and eliminates any
	// possibility of SQL malicious code injection.

	// --- Create the template SQL query for the prepared statement.

	$query = "UPDATE ns_marriages_data SET $field[0]=?";
	for ($i=1; $i<count($field); $i++) $query = $query . ", $field[$i]=?";
	$query = $query . " WHERE MarriageID=$MarriageID";
	
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
	
	$query = "SELECT * FROM ns_marriages_data WHERE MarriageID = $MarriageID";
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

// Get the PGV data on the bride and groom.

$pgv_bride = new pgv_ind($db, $n_id_b);
$pgv_groom = new pgv_ind($db, $n_id_g);

// Before generating the form, convert any characters in the strings that have special
// significance in HTML to the correct HTML entities

for ($i=0; $i<count($field); $i++) ${$field[$i]} = htmlspecialchars(${$field[$i]});

// Generate the form.

echo '<form action="edit_ns_marriages_data.php" method="post">' . PHP_EOL;

echo <<<EOT
<div style="width: 100%; overflow: hidden;" >
	<div style="width: 50%; float: left;">
		<label for="date">Marriage Date</label>
		<input type="text" name="date" id="date" value="$date" >
		<label for="place">Marriage Place</label>
		<input type="text" name="place" id="place" value="$place" >
	</div>
	<div style="width: 50%; float: right; text-align: center; " >
		<br>
		<span style="white-space: nowrap">
			<label for="n_id_g">Groom n_id</label>
			<input type="text" name="n_id_g" id="n_id_g" value="$n_id_g"  style="width:100px; min-width: 0;" >
		</span>
		<span style="white-space: nowrap">
			<label for="n_id_b">Bride n_id</label>
			<input type="text" name="n_id_b" id="n_id_b" value="$n_id_b"  style="width:100px; min-width: 0; ">
		</span>
	</div>
</div>
<br>
EOT;

table_start();

$cell[0] = NULL;
$cell[1] = 'Groom NSHVS';
$cell[2] = 'Groom Database';
$cell[3] = 'Bride NSHVS';
$cell[4] = 'Bride Database';
table_row_header($cell);

$cell[0] = 'Name';
$cell[1] = form_input('groom');
$cell[2] = $pgv_groom->link;
$cell[3] = form_input('bride');
$cell[4] = $pgv_bride->link;
table_row($cell);

$cell[0] = 'Age';
$cell[1] = form_input('groom_age');
$cell[2] = null;
$cell[3] = form_input('bride_age');
$cell[4] = null;
table_row($cell);

$cell[0] = 'Status';
$cell[1] = form_input('groom_status');
$cell[2] = null;
$cell[3] = form_input('bride_status');
$cell[4] = null;
table_row($cell);

$cell[0] = 'Residence';
$cell[1] = form_input('groom_residence');
$cell[2] = null;
$cell[3] = form_input('bride_residence');
$cell[4] = null;
table_row($cell);

$cell[0] = 'Birthplace';
$cell[1] = form_input('groom_birthplace');
$cell[2] = $pgv_groom->birth_place;
$cell[3] = form_input('bride_birthplace');
$cell[4] = $pgv_bride->birth_place;
table_row($cell);

$cell[0] = 'Occupation';
$cell[1] = form_input('groom_occupation');
$cell[2] = null;
$cell[3] = form_input('bride_occupation');
$cell[4] = null;
table_row($cell);

$cell[0] = 'Father';
$cell[1] = form_input('groom_father');
$cell[2] = $pgv_groom->father_link;
$cell[3] = form_input('bride_father');
$cell[4] = $pgv_bride->father_link;
table_row($cell);

$cell[0] = "Father's Birthplace";
$cell[1] = form_input('groom_father_birthplace');
$cell[2] = null;
$cell[3] = form_input('bride_father_birthplace');
$cell[4] = null;
table_row($cell);

$cell[0] = 'Mother';
$cell[1] = form_input('groom_mother');
$cell[2] = $pgv_groom->mother_link;
$cell[3] = form_input('bride_mother');
$cell[4] = $pgv_bride->mother_link;
table_row($cell);

$cell[0] = "Mother's Birthplace";
$cell[1] = form_input('groom_mother_birthplace');
$cell[2] = null;
$cell[3] = form_input('bride_mother_birthplace');
$cell[4] = null;
table_row($cell);

table_end();

echo <<<EOT
<label for="notes">Notes</label>
<input type="text" name="notes" id="notes" value="$notes" >
<br><br>
<input type="submit" name="submit" value="Submit" >
<input type="hidden" name="MarriageID" value="$MarriageID" >
<input type="hidden" name="new_record" value="$new_record">
</form>
</body></html>
EOT;

?>

