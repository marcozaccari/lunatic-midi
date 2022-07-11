<?php

$image_filename = $argv[1];
if (!file_exists($image_filename))
    die("$image_filename does not exists\n");

$img = imagecreatefrompng($image_filename);

if (!$img)
    die("Cannot load $image_filename\n");

$output_filename = substr($image_filename, 0, strrpos($image_filename, '.')).'.dat';
$f = fopen($output_filename, "w");

$width = imagesx($img);
$height = imagesy($img);

$value = 0;
for ($x = 0; $x < $width ; $x++) {
    $found = false;

    for ($y = 0; $y < $height; $y++) {

	$color = imagecolorat($img, $x, $y);

	if ($color == 0) {
	    $value = $height - $y;
	    $found = true;
	    fwrite($f, chr($value));
	    break;
	}
    }

    if (!$found) {
	echo("  warning: no pixels at $x column. Using last value\n");
	fwrite($f, chr($value));
    }
}

fclose($f);