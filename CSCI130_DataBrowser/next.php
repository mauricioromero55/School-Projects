<?php

$servername = "localhost";  // Replace with your MySQL server address
$username = "AdminLab11";  // Replace with your MySQL username
$password = "4VPnroTOC6wOU3mn";  // Replace with your MySQL password
$database = "pokemon_database";

// Create a new mysqli connection
$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

//pokemon is table inside pokemon_database
$sql = "SELECT name, type, hp, atk, spA, def, spD, sp FROM pokemon_database"

if(!isset($_SESSION['currentIndex'])){
    $_SESSION['currentIndex']=0;
}

if($_SERVER["REQUEST_METHOD"]=="POST"){
    $boolNext = isset($_POST['boolNext']) ? filter_var($_POST['boolNext'], FILTER_VALIDATE_BOOLEAN): false;
        if($boolNext){
            $_SESSION['currentIndex'] = min(count($carsArr) - 1, $_SESSION['currentCarIndex'] + 1);
            $nextIndex = $_SESSION['currentIndex'];
            $nextP = $carsArr[$nextCarIndex];

    // Send the next car details along with its index
             echo json_encode(['index' => $nextCarIndex, 'car' => $nextCar]);
            exit();
    }
}

?>