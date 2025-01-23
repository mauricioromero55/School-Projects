<?php 
$servername = "localhost";  // Replace with your MySQL server address
$username = "AdminLab11";  // Replace with your MySQL username
$password = "4VPnroTOC6wOU3mn";  // Replace with your MySQL password
$db_name = "pokemon_db";
// Create connection
$conn = new mysqli($servername, $username, $password, $db_name);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Select only the id column and order by name
$sqlSelect = "SELECT id FROM pokemon_table ORDER BY name";

$result = $conn->query($sqlSelect);
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

// Close the connection
$conn->close();
?>