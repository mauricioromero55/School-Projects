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

// Perform a query to fetch primary keys from the database
$sql = "SELECT id FROM pokemon_table ORDER BY id";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Fetch primary keys and store them in an array
    $primaryKeys = [];
    while ($row = $result->fetch_assoc()) {
        $primaryKeys[] = $row['id'];
    }
    // Convert the array to JSON and echo it
    echo json_encode($primaryKeys);
} else {
    echo "No results found.";
}

$conn->close();

?>