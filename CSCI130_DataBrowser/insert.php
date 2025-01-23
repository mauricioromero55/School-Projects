<?php

$servername = "localhost";  // Replace with your MySQL server address
$username = "AdminLab11";  // Replace with your MySQL username
$password = "4VPnroTOC6wOU3mn";  // Replace with your MySQL password
$db_name = "pokemon_db";

// Create a new mysqli connection
$conn = new mysqli($servername, $username, $password, $db_name);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$data = json_decode(file_get_contents("php://input"), true);
if ($data === null) {
    echo "Error decoding JSON data<br>";
    // Handle the error as needed
} else {
    // Insert data into the database
    $name = $data[1];
    $type = $data[2];
    $gender = $data[3];
    $hp = $data[4];
    $atk = $data[5];
    $spA= $data[6];
    $def = $data[7];
    $spD = $data[8];
    $speed = $data[9];
    $image = $data[10];
    $sql = "INSERT INTO pokemon_table (name, type, gender, hp, attack, defense, spAttack, spDefense, speed, image) VALUES 
    ('$name', '$type', '$gender', '$hp', '$atk', '$def', '$spA', '$spD', '$speed', '$image')
    ON DUPLICATE KEY UPDATE name = VALUES(name), type = VALUES(type), 
    gender = VALUES(gender), hp = VALUES(hp), attack = VALUES(attack), 
    defense = VALUES(defense), spAttack = VALUES(spAttack), 
    spDefense = VALUES(spDefense), speed = VALUES(speed), image = VALUES(image)";  

    if ($conn->query($sql) === TRUE) {
        echo "Row inserted successfully<br>";
    } else {
        echo "Error inserting row: " . $conn->error . "<br>";
    }
}

// Close the database connection
$conn->close();


?>