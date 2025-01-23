<?php

// You should have file_uploads = On in C:\xampp\php\php.ini (if you have xampp)

$target_dir = "uploads/"; // you must create this directory in the folder where you have the PHP file
$target_file = $target_dir . basename($_FILES["fileupload"]["name"]);

//echo "<p>Upload information</p><ul>";
//echo  "<li>Target folder for the upload :". $target_file . "</li>";
//echo  "<li>File name :". basename($_FILES["fileup"]["name"]) . "</li>";

// basename: Returns the base name of the given path

$uploadOk = 1;

$imageFileType = pathinfo($target_file,PATHINFO_EXTENSION);
// Verify if the image file is an actual image or a fake image
if(isset($_POST["submit"])) {
    $check = getimagesize($_FILES["fileupload"]["tmp_name"]);
    if($check !== false) {
       // echo "<li>File is an image of type - " . $check["mime"] . ".</li>";
        $uploadOk = 1;
    } else {
        echo "<li>File is not an image.</li>";
        $uploadOk = 0;
    }
}
// Verify if file already exists
if (file_exists($target_file)) {
 //   echo "<li>The file already exists.</li>";
   // $uploadOk = 0;
}
// Verify the file size
if ($_FILES["fileupload"]["size"] > 500000) {
    echo "<li>The file is too large.</li>";
    $uploadOk = 0;
}
// Verify certain file formats
if($imageFileType != "jpg" && $imageFileType != "png") {
    echo "<li>Only jpg and png files are allowed for the upload.</li>";
    $uploadOk = 0;
}
// Verify if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
    echo "<li>The file was not uploaded.</li>";
} else { // upload file
    if (move_uploaded_file($_FILES["fileupload"]["tmp_name"], $target_file)) {
 //       echo "<li>The file ". basename( $_FILES["fileup"]["name"]). " has been uploaded.</li>";
        echo json_encode(["image" => $target_file]);
    } else {
        echo "<li>Error uploading your file.</li>";
    }
}

//echo "</ul>";



?>