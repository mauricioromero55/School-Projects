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
    if (!is_array($data)) {
        echo "Error decoding JSON data array<br>";
        // Handle the error as needed
    } else {
        // Select all rows from the table
        $sql = "SELECT * FROM pokemon_table";
        $result = $conn->query($sql);

        // Check if there are rows in the result set
        if ($result->num_rows > 0) {
            // Loop through each row
            $pokeID = $data[0];

            while ($row = $result->fetch_assoc()) {
                // Access the columns of the current row
                //$name = $row["name"];
                //$type = $row["type"];
                //$hp = $row["hp"];
                $id = $row["id"];
                if($pokeID == $id) {
                    $newName = $data[1];
                    $newType = $data[2];
                    $newGen = $data[3];
                    $newHp = $data[4];
                    $newAtk = $data[5];
                    $newSpA = $data[6];
                    $newDef = $data[7];
                    $newSpD = $data[8];
                    $newSpeed = $data[9];

                    // Update the record in the database
                    $sql = "UPDATE pokemon_table 
                            SET name = '$newName', type = '$newType', gender = '$newGen',
                            hp = $newHp, attack = $newAtk, spAttack = $newSpA, 
                            defense = $newDef, spDefense = $newSpD, speed = $newSpeed 
                            WHERE id = $pokeID";

                    if ($conn->query($sql) === TRUE) {
                        echo "Record updated successfully";
                    } else {
                        echo "Error updating record: " . $conn->error;
                    }
                }
            }
        } else {
           echo "0 results";
        }
    }
}

$conn->close();

?>