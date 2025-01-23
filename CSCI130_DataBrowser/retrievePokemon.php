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

//$pokeID = json_decode(file_get_contents("php://input"), true);

$pokeID = $_GET['id'];

// Create a SQL query to retrieve the row with the specified ID
$sql = "SELECT * FROM pokemon_table WHERE id = $pokeID";

$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // Fetch the row as an associative array
    $row = $result->fetch_assoc();

    // Convert the associative array to JSON and output
    echo json_encode($row);
} else {
    echo "No Pokemon found with the specified ID";
}

$conn->close();

?>