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

$data = json_decode(file_get_contents("php://input"), true);

if ($data === null) {
    echo "Error decoding JSON data<br>";
    // Handle the error as needed
} else {
    // Select all rows from the table
    $sql = "SELECT * FROM pokemon_table";
    $result = $conn->query($sql);

    // Check if there are rows in the result set
    if ($result->num_rows > 0) {
        // Loop through each row
        $pokeID = $data;

        while ($row = $result->fetch_assoc()) {
            $id = $row["id"];
            if($pokeID == $id) {
                // Prepare the DELETE SQL statement
                $sql = "DELETE FROM pokemon_table WHERE id = $pokeID";

                // Execute the DELETE query
                if ($conn->query($sql) === TRUE) {
                    echo "Record deleted successfully";
                } else {
                    echo "Error deleting record: " . $conn->error;
                }
            }
        }
    } else {
       echo "0 results";
    }
    
}

$conn->close();

?>