<?php

$servername = "localhost";
$username = "AdminLab11";
$password = "4VPnroTOC6wOU3mn";
$dbname = "pokemon_db";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Create a SQL query to count the number of rows in the table
$sql = "SELECT COUNT(*) as total FROM pokemon_table";

$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Fetch the row as an associative array
    $row = $result->fetch_assoc();

    // Output the total number of rows
    echo $row['total'];
} else {
    echo "Error counting rows";
}

$conn->close();

?>