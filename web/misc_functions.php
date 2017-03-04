<?php

/**
* @file
*
* @brief Basic HTML functions.
*
* @date 1 March 2017
*
*/


/**
* @brief Generate HTML page header block.
*
* This function generates a header block that meets the "HTML 4.01 Transitional" standard.
*
* @param[in] $title     web page title
* @param[in] $css_file  CSS file (optional)
* @param[in] $icon_lib  icon library to load (optional)
*
*/

function html_header ($title, $css_file = null, $icon_lib = null)
{
echo <<<EOT
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta content="text/html; charset=ISO-8859-1"
http-equiv="content-type">
<title>$title</title>

EOT;

if ($icon_lib == 'Google')
	echo '<link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">' . PHP_EOL;

if (isset($css_file)) 
{
echo <<<EOT
<link rel="stylesheet" type="text/css" href="$css_file">
</head>
<body>
EOT;
}
else
{
echo <<<EOT
</head>
<body>
EOT;
}
echo PHP_EOL;
}

/**
*
* @brief Generate HTML page trailer block.
*
*/

function html_trailer()
{
echo <<<EOT
</body>
</html>
EOT;
}

/**
* @brief Generate HTML table header.
*/

function table_start()
{
echo <<<EOT
<table style="text-align: left; width: 100%;" border="1" cellpadding="2"
cellspacing="2">
<tbody>
EOT;
echo PHP_EOL;
}

/**
* @brief Generate HTML table trailer.
*/
function table_end()
{
echo <<<EOT
</tbody>
</table>
<br>
EOT;
echo PHP_EOL;
}

/**
* @brief Generate one header row in a table.
*
* @param[in] $cells   an array containing the contents of the cells in the row
*/
function table_row_header($cells)
{
	echo '<tr>'.PHP_EOL;
	foreach ($cells as $cell)
	{
		echo '<th align="center">'.$cell.'</th>'.PHP_EOL;
	}
	echo '</tr>'.PHP_EOL;
}

/**
* @brief Generate one row in a table.
*
* @param[in] $cells   an array containing the contents of the cells in the row
*/
function table_row($cells)
{
	echo '<tr>'.PHP_EOL;
	foreach ($cells as $cell)
	{
		echo '<td>'.$cell.'</td>'.PHP_EOL;
	}
	echo '</tr>'.PHP_EOL;
}

/**
* @brief Generate HTML link.
*
* This function first combines the values in `$text` and `$url` to form the link.
* Then any characters that have special significance in HTML are converted
* to HTML entities to preserve their meanings.
* The value of `$url` must either be a fully formed URL string or be `NULL`.
* If `$url` is `NULL`, then the return value is set to `$text`.
*
* @param[in] $text   human readable part of the link
* @param[in] $url    URL for the link
*
* @return the HTML link
*/

function html_link($text, $url)
{
	if ($url == null)
	{
		return htmlspecialchars($text);
	}
	else
	{
		return '<a href="' . htmlspecialchars($url) . '">' . $text . '</a>';
	}
}

?> 
