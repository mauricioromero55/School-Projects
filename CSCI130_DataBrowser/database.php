<?php 
$servername = "localhost";
$username = "AdminLab11";
$password = "4VPnroTOC6wOU3mn";
$dbname = "pokemon_db";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Create the database
$sql = "CREATE DATABASE IF NOT EXISTS $dbname";
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully or already exists<br>";
} else {
    echo "Error creating database: " . $conn->error . "<br>";
}

$conn->close();

// Create a new connection to the specified database
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Create the table
$sql = "CREATE TABLE IF NOT EXISTS pokemon_table (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    type VARCHAR(255) NOT NULL,
    gender VARCHAR(255) NOT NULL,
    hp INT NOT NULL,
    attack INT NOT NULL,
    defense INT NOT NULL,
    spAttack INT NOT NULL,
    spDefense INT NOT NULL,
    speed INT NOT NULL,
    image VARCHAR(255) NOT NULL,
    UNIQUE KEY unique_pokemon (name, type)
)";

// Check the success of creating the table
if ($conn->query($sql) === TRUE) {
    echo "Table created successfully or already exists<br>";
    $sqlResetAutoIncrement = "ALTER TABLE pokemon_table AUTO_INCREMENT = 1";
    if ($conn->query($sqlResetAutoIncrement) === TRUE) {
        echo "Auto-increment reset successfully<br>";
    } else {
        echo "Error resetting auto-increment: " . $conn->error . "<br>";
    }
    $dataArray = null;
    $data = json_decode(file_get_contents("php://input"), true);
    var_dump($data);
    if ($data === null) {
        echo "Error decoding JSON data<br>";
        // Handle the error as needed
    } else {
        $dataArray = json_decode($data['data'], true);
        var_dump($dataArray);
        if (!is_array($dataArray)) {
            echo "Error decoding JSON data array<br>";
            // Handle the error as needed
        } else {
            // Now you can proceed with processing $dataArray
            // Loop to insert data into the database
            // Loop to insert data into the database
            for($i = 0; $i < count($dataArray); $i++) {
                $curr = $dataArray[$i];
                $name = $conn->real_escape_string($curr['name']);
                $type = $conn->real_escape_string($curr['type']);
                $gender = $conn->real_escape_string($curr['gender']);
                $hp =  $curr['hp'];
                $atk =  $curr['atk'];
                $def =  $curr['def'];
                $spA =  $curr['spA'];
                $spD =  $curr['spD'];
                $speed =  $curr['speed'];
                $image =  $curr['img'];
        
                // Insert data into the database
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
        }
    }
} else {
    echo "Error creating table: " . $conn->error . "<br>";
}

$conn->close();
?>
