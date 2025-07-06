<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_data";

$conn = new mysqli($servername, $username, $password);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
$sql = "CREATE DATABASE IF NOT EXISTS $dbname";
if ($conn->query($sql) !== TRUE) {
    die("Error creating database: " . $conn->error);
}
$conn->close();

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "CREATE TABLE IF NOT EXISTS text_history (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    receiver VARCHAR(10) NOT NULL,
    text VARCHAR(50) NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)";
if ($conn->query($sql) !== TRUE) {
    die("Error creating table: " . $conn->error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $receiver = $_POST["receiver"];
    $text = $_POST["text"];

    $receiver = mysqli_real_escape_string($conn, $receiver);
    $text = mysqli_real_escape_string($conn, $text);

    $sql = "INSERT INTO text_history (receiver, text) VALUES ('$receiver', '$text')";
    if ($conn->query($sql) !== TRUE) {
        die("Error: " . $sql . "<br>" . $conn->error);
    }
}

$sql = "SELECT id, receiver, text, timestamp FROM text_history ORDER BY timestamp DESC";
$result = $conn->query($sql);
$history = [];
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $history[] = $row;
    }
}

header('Content-Type: application/json');
echo json_encode($history);

$conn->close();
